//===-- Comet2RegisterInfo.cpp - COMET-II Register Information --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the COMET-II implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "Comet2RegisterInfo.h"
#include "Comet2.h"
#include "Comet2Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "comet2-reginfo"

#define GET_REGINFO_ENUM
#define GET_REGINFO_TARGET_DESC
#include "Comet2GenRegisterInfo.inc"

using namespace llvm;

Comet2RegisterInfo::Comet2RegisterInfo(unsigned HwMode)
    : Comet2GenRegisterInfo(Comet2::GR0, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0, HwMode) {}

const MCPhysReg *
Comet2RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

BitVector Comet2RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, Comet2::GR0); // return
  markSuperRegs(Reserved, Comet2::GR7); // stack pointer
  markSuperRegs(Reserved, Comet2::FR);  // flag register
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

void Comet2RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  LLVM_DEBUG(dbgs() << "### eliminateFrameIndex 0\n");

  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  DebugLoc DL = MI.getDebugLoc();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  unsigned FrameReg;
  int Offset =
      getFrameLowering(MF)->getFrameIndexReference(MF, FrameIndex, FrameReg) +
      MI.getOperand(FIOperandNum + 1).getImm();

  if (!isInt<16>(Offset)) {
    report_fatal_error(
        "Frame offsets outside of the signed 16-bit range not supported");
  }

  MI.getOperand(FIOperandNum)
      .ChangeToRegister(FrameReg, false);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
}

Register Comet2RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Comet2::GR7;
}

const uint32_t *
Comet2RegisterInfo::getCallPreservedMask(const MachineFunction & MF,
                                        CallingConv::ID /*CC*/) const {
  return CSR_RegMask;
}
