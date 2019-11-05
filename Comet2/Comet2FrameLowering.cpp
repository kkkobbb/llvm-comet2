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

  // allocate fixed size for simplicity
  uint64_t StackSize = 4 * 16;

   // Update stack size
  MFI->setStackSize(StackSize);

  // スタックを1フレーム分伸ばす
  BuildMI(MBB, MBBI, DL, TII.get(Comet2::LAD), Comet2::GR1)
      .addImm(-StackSize);
  BuildMI(MBB, MBBI, DL, TII.get(Comet2::ADDA), Comet2::GR7)
      .addReg(Sample::GR1);
}

void Comet2FrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  DebugLoc DL = MBBI->getDebugLoc();
  const Comet2VInstrInfo *TII = STI.getInstrInfo();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  // スタックを1フレーム分戻す
  BuildMI(MBB, MBBI, DL, TII.get(Comet2::LAD), Comet2::GR1)
      .addImm(StackSize);
  BuildMI(MBB, MBBI, DL, TII.get(Comet2::SUBA), Comet2::GR7)
      .addReg(Sample::GR1);
}

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