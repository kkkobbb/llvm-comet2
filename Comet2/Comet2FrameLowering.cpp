//===-- Comet2FrameLowering.cpp - COMET-II Frame Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the COMET-II implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Comet2FrameLowering.h"
#include "Comet2MachineFunctionInfo.h"
#include "Comet2Subtarget.h"
#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/MC/MCDwarf.h"

using namespace llvm;

bool Comet2FrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

void Comet2FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  MachineFrameInfo &MFI = MF.getFrameInfo();
  const Comet2InstrInfo *TII = STI.getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = MBBI->getDebugLoc();

  uint64_t StackSize = MFI.getStackSize();

  // Early exit if there is no need to allocate on the stack
  if (StackSize == 0 && !MFI.adjustsStack())
    return;


   // Update stack size
  MFI.setStackSize(StackSize);

  // スタックを1フレーム分伸ばす
  Register tmp = Comet2::GR6;
  BuildMI(MBB, MBBI, DL, TII->get(Comet2::LAD), tmp)
      .addImm(StackSize);
  BuildMI(MBB, MBBI, DL, TII->get(Comet2::SUBAREG), Comet2::GR7)
      .addReg(tmp);
}

void Comet2FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  DebugLoc DL = MBBI->getDebugLoc();
  const Comet2InstrInfo *TII = STI.getInstrInfo();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI.getStackSize();

  // スタックを1フレーム分戻す
  Register tmp = Comet2::GR6;
  BuildMI(MBB, MBBI, DL, TII->get(Comet2::LAD), tmp)
      .addImm(StackSize);
  BuildMI(MBB, MBBI, DL, TII->get(Comet2::ADDAREG), Comet2::GR7)
      .addReg(tmp);
}








bool Comet2FrameLowering::canSimplifyCallFramePseudos(
    const MachineFunction &MF) const {
  // Always simplify call frame pseudo instructions, even when
  // hasReservedCallFrame is false.
  return true;
}

bool Comet2FrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  // Reserve call frame memory in function prologue under the following
  // conditions:
  // - Y pointer is reserved to be the frame pointer.
  // - The function does not contain variable sized objects.

  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return hasFP(MF) && !MFI.hasVarSizedObjects();
}

bool Comet2FrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    const std::vector<CalleeSavedInfo> &CSI,
    const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  unsigned CalleeFrameSize = 0;
  DebugLoc DL = MBB.findDebugLoc(MI);
  MachineFunction &MF = *MBB.getParent();
  const Comet2Subtarget &STI = MF.getSubtarget<Comet2Subtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();
  Comet2MachineFunctionInfo *MFI = MF.getInfo<Comet2MachineFunctionInfo>();

  for (unsigned i = CSI.size(); i != 0; --i) {
    unsigned Reg = CSI[i - 1].getReg();
    bool IsNotLiveIn = !MBB.isLiveIn(Reg);

    assert(TRI->getRegSizeInBits(*TRI->getMinimalPhysRegClass(Reg)) == 8 &&
           "Invalid register size");

    // Add the callee-saved register as live-in only if it is not already a
    // live-in register, this usually happens with arguments that are passed
    // through callee-saved registers.
    if (IsNotLiveIn) {
      MBB.addLiveIn(Reg);
    }

    // Do not kill the register when it is an input argument.
    // TODO 引数push
    /*
    BuildMI(MBB, MI, DL, TII.get(AVR::PUSHRr))
        .addReg(Reg, getKillRegState(IsNotLiveIn))
        .setMIFlag(MachineInstr::FrameSetup);
        */
    ++CalleeFrameSize;
  }

  // TODO 不要？
  //MFI->setCalleeSavedFrameSize(CalleeFrameSize);

  return true;
}

bool Comet2FrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    std::vector<CalleeSavedInfo> &CSI,
    const TargetRegisterInfo *TRI) const {
  if (CSI.empty()) {
    return false;
  }

  DebugLoc DL = MBB.findDebugLoc(MI);
  const MachineFunction &MF = *MBB.getParent();
  const Comet2Subtarget &STI = MF.getSubtarget<Comet2Subtarget>();
  const TargetInstrInfo &TII = *STI.getInstrInfo();

  for (const CalleeSavedInfo &CCSI : CSI) {
    unsigned Reg = CCSI.getReg();

    assert(TRI->getRegSizeInBits(*TRI->getMinimalPhysRegClass(Reg)) == 8 &&
           "Invalid register size");

    // TODO 引数pop
    //BuildMI(MBB, MI, DL, TII.get(AVR::POPRd), Reg);
  }

  return true;
}

/// Replace pseudo store instructions that pass arguments through the stack with
/// real instructions. If insertPushes is true then all instructions are
/// replaced with push instructions, otherwise regular std instructions are
/// inserted.
static void fixStackStores(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI,
                           const TargetInstrInfo &TII, bool insertPushes) {
  const Comet2Subtarget &STI = MBB.getParent()->getSubtarget<Comet2Subtarget>();
  const TargetRegisterInfo &TRI = *STI.getRegisterInfo();

  // TODO 未実装
#if 0
  // Iterate through the BB until we hit a call instruction or we reach the end.
  for (auto I = MI, E = MBB.end(); I != E && !I->isCall();) {
    MachineBasicBlock::iterator NextMI = std::next(I);
    MachineInstr &MI = *I;
    unsigned Opcode = I->getOpcode();

    // Only care of pseudo store instructions where SP is the base pointer.
    // TODO 不要？
    /*
    if (Opcode != AVR::STDSPQRr && Opcode != AVR::STDWSPQRr) {
      I = NextMI;
      continue;
    }

    assert(MI.getOperand(0).getReg() == AVR::SP &&
           "Invalid register, should be SP!");
    */
    if (insertPushes) {
      // Replace this instruction with a push.
      unsigned SrcReg = MI.getOperand(2).getReg();
      bool SrcIsKill = MI.getOperand(2).isKill();

      // We can't use PUSHWRr here because when expanded the order of the new
      // instructions are reversed from what we need. Perform the expansion now.
      if (Opcode == AVR::STDWSPQRr) {
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
            .addReg(TRI.getSubReg(SrcReg, AVR::sub_hi),
                    getKillRegState(SrcIsKill));
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
            .addReg(TRI.getSubReg(SrcReg, AVR::sub_lo),
                    getKillRegState(SrcIsKill));
      } else {
        BuildMI(MBB, I, MI.getDebugLoc(), TII.get(AVR::PUSHRr))
            .addReg(SrcReg, getKillRegState(SrcIsKill));
      }

      MI.eraseFromParent();
      I = NextMI;
      continue;
    }

    // Replace this instruction with a regular store. Use Y as the base
    // pointer since it is guaranteed to contain a copy of SP.
    unsigned STOpc =
        (Opcode == AVR::STDWSPQRr) ? AVR::STDWPtrQRr : AVR::STDPtrQRr;

    MI.setDesc(TII.get(STOpc));
    MI.getOperand(0).setReg(AVR::R29R28);

    I = NextMI;
  }
#endif
}

MachineBasicBlock::iterator Comet2FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
  const Comet2Subtarget &STI = MF.getSubtarget<Comet2Subtarget>();
  const Comet2InstrInfo &TII = *STI.getInstrInfo();

  // There is nothing to insert when the call frame memory is allocated during
  // function entry. Delete the call frame pseudo and replace all pseudo stores
  // with real store instructions.
  if (hasReservedCallFrame(MF)) {
    fixStackStores(MBB, MI, TII, false);
    return MBB.erase(MI);
  }

  DebugLoc DL = MI->getDebugLoc();
  unsigned int Opcode = MI->getOpcode();
  int Amount = TII.getFrameSize(*MI);

  // Adjcallstackup does not need to allocate stack space for the call, instead
  // we insert push instructions that will allocate the necessary stack.
  // For adjcallstackdown we convert it into an 'adiw reg, <amt>' handling
  // the read and write of SP in I/O space.
  if (Amount != 0) {
    assert(getStackAlignment() == 1 && "Unsupported stack alignment");

    if (Opcode == TII.getCallFrameSetupOpcode()) {
      fixStackStores(MBB, MI, TII, true);
    } else {
      assert(Opcode == TII.getCallFrameDestroyOpcode());

      // TODO 未実装
#if 0
      // Select the best opcode to adjust SP based on the offset size.
      unsigned addOpcode;
      if (isUInt<6>(Amount)) {
        addOpcode = AVR::ADIWRdK;
      } else {
        addOpcode = AVR::SUBIWRdK;
        Amount = -Amount;
      }

      // Build the instruction sequence.
      BuildMI(MBB, MI, DL, TII.get(AVR::SPREAD), AVR::R31R30).addReg(AVR::SP);

      MachineInstr *New = BuildMI(MBB, MI, DL, TII.get(addOpcode), AVR::R31R30)
                              .addReg(AVR::R31R30, RegState::Kill)
                              .addImm(Amount);
      New->getOperand(3).setIsDead();

      BuildMI(MBB, MI, DL, TII.get(AVR::SPWRITE), AVR::SP)
          .addReg(AVR::R31R30, RegState::Kill);
#endif
    }
  }

  return MBB.erase(MI);
}

void Comet2FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);

  // If we have a frame pointer, the Y register needs to be saved as well.
  // We don't do that here however - the prologue and epilogue generation
  // code will handle it specially.
}

/// The frame analyzer pass.
///
/// Scans the function for allocas and used arguments
/// that are passed through the stack.
struct Comet2FrameAnalyzer : public MachineFunctionPass {
  static char ID;
  Comet2FrameAnalyzer() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) {
    const MachineFrameInfo &MFI = MF.getFrameInfo();
    Comet2MachineFunctionInfo *FuncInfo = MF.getInfo<Comet2MachineFunctionInfo>();

    // If there are no fixed frame indexes during this stage it means there
    // are allocas present in the function.
    if (MFI.getNumObjects() != MFI.getNumFixedObjects()) {
      // Check for the type of allocas present in the function. We only care
      // about fixed size allocas so do not give false positives if only
      // variable sized allocas are present.
      for (unsigned i = 0, e = MFI.getObjectIndexEnd(); i != e; ++i) {
        // Variable sized objects have size 0.
        if (MFI.getObjectSize(i)) {
          //FuncInfo->setHasAllocas(true);
          break;
        }
      }
    }

    // If there are fixed frame indexes present, scan the function to see if
    // they are really being used.
    if (MFI.getNumFixedObjects() == 0) {
      return false;
    }

    // Ok fixed frame indexes present, now scan the function to see if they
    // are really being used, otherwise we can ignore them.
    for (const MachineBasicBlock &BB : MF) {
      for (const MachineInstr &MI : BB) {
        int Opcode = MI.getOpcode();

        // TODO load store
        /*
        if ((Opcode != AVR::LDDRdPtrQ) && (Opcode != AVR::LDDWRdPtrQ) &&
            (Opcode != AVR::STDPtrQRr) && (Opcode != AVR::STDWPtrQRr)) {
          continue;
        }
        */

        for (const MachineOperand &MO : MI.operands()) {
          if (!MO.isFI()) {
            continue;
          }

          if (MFI.isFixedObjectIndex(MO.getIndex())) {
            // TODO 不要？
            //FuncInfo->setHasStackArgs(true);
            return false;
          }
        }
      }
    }

    return false;
  }

  StringRef getPassName() const { return "Comet2 Frame Analyzer"; }
};

char Comet2FrameAnalyzer::ID = 0;

/// Creates instance of the frame analyzer pass.
FunctionPass *createAVRFrameAnalyzerPass() { return new Comet2FrameAnalyzer(); }






// NOTE 実装あり llvm/lib/CodeGen/TargetFrameLoweringImpl.cpp
#if 0
int Comet2FrameLowering::getFrameIndexReference(const MachineFunction &MF,
                                               int FI,
                                               unsigned &FrameReg) const {
}
#endif

// NOTE 実装あり llvm/lib/CodeGen/TargetFrameLoweringImpl.cpp
#if 0
void Comet2FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                              BitVector &SavedRegs,
                                              RegScavenger *RS) const {
}
#endif

// NOTE 実装あり llvm/include/llvm/CodeGen/TargetFrameLowering.h
#if 0
void Comet2FrameLowering::processFunctionBeforeFrameFinalized(
    MachineFunction &MF, RegScavenger *RS) const {
}
#endif

// NOTE 実装あり llvm/include/llvm/CodeGen/TargetFrameLowering.h
#if 0
// Not preserve stack space within prologue for outgoing variables when the
// function contains variable size objects and let eliminateCallFramePseudoInstr
// preserve stack space for it.
bool Comet2FrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return !MF.getFrameInfo().hasVarSizedObjects();
}
#endif

// NOTE 実装あり llvm/include/llvm/CodeGen/TargetFrameLowering.h
#if 0
// Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions.
MachineBasicBlock::iterator Comet2FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {
}
#endif
