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

// Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions.
MachineBasicBlock::iterator Comet2FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {

  if (!hasReservedCallFrame(MF)) {
    // If space has not been reserved for a call frame, ADJCALLSTACKDOWN and
    // ADJCALLSTACKUP must be converted to instructions manipulating the stack
    // pointer. This is necessary when there is a variable length stack
    // allocation (e.g. alloca), which means it's not possible to allocate
    // space for outgoing arguments from within the function prologue.
    int64_t Amount = MI->getOperand(0).getImm();

    if (Amount != 0) {
      // Ensure the stack remains aligned after adjustment.
      // TODO 未実装
    }
  }

  return MBB.erase(MI);
}
