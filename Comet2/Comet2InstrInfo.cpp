//===-- Comet2InstrInfo.cpp - COMET-II Instruction Information --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the COMET-II implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Comet2InstrInfo.h"
#include "Comet2.h"
#include "Comet2Subtarget.h"
#include "Comet2TargetMachine.h"
#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "Comet2GenInstrInfo.inc"

using namespace llvm;

Comet2InstrInfo::Comet2InstrInfo()
    : Comet2GenInstrInfo(Comet2::ADJCALLSTACKDOWN, Comet2::ADJCALLSTACKUP) {}

unsigned Comet2InstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case Comet2::LD:
    break;
  }

  if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
      MI.getOperand(2).getImm() == 0) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }

  return 0;
}

unsigned Comet2InstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  switch (MI.getOpcode()) {
  default:
    return 0;
  case Comet2::ST:
    break;
  }

  if (MI.getOperand(0).isFI() && MI.getOperand(1).isImm() &&
      MI.getOperand(1).getImm() == 0) {
    FrameIndex = MI.getOperand(0).getIndex();
    return MI.getOperand(2).getReg();
  }

  return 0;
}

void Comet2InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, unsigned DstReg,
                                  unsigned SrcReg, bool KillSrc) const {
  BuildMI(MBB, MBBI, DL, get(Comet2::LDREG))
      .addReg(DstReg, RegState::Define)
      .addReg(SrcReg, getKillRegState(KillSrc));
}

void Comet2InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned SrcReg, bool IsKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  BuildMI(MBB, I, DL, get(Comet2::ST))
      .addReg(SrcReg, getKillRegState(IsKill))
      .addFrameIndex(FI)
      .addImm(0);  // TODO フレームインデックスの指定はこれでいい？
}

void Comet2InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator I,
                                           unsigned DstReg, int FI,
                                           const TargetRegisterClass *RC,
                                           const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  BuildMI(MBB, I, DL, get(Comet2::LD), DstReg)
      .addFrameIndex(FI)
      .addImm(0);  // TODO フレームインデックスの指定はこれでいい？
}

static unsigned getOppositeBranchOpcode(int Opc) {
  // TODO 分岐なし
  llvm_unreachable("Unrecognized conditional branch");
}

bool Comet2InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  // TODO branch etc
  // 未実装の場合はtrueを返す
  return true;
}

unsigned Comet2InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                      int *BytesRemoved) const {
  // TODO branch etc
  llvm_unreachable("Target doesn't implement Comet2InstrInfo::removeBranch");
}

unsigned Comet2InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  // TODO branch etc
  llvm_unreachable("Target doesn't implement Comet2InstrInfo::insertBranch!");
}

unsigned Comet2InstrInfo::insertIndirectBranch(MachineBasicBlock &MBB,
                                               MachineBasicBlock &DestBB,
                                               const DebugLoc &DL,
                                               int64_t BrOffset,
                                               RegScavenger *RS) const {
  // TODO branch etc
  llvm_unreachable("Target doesn't implement Comet2InstrInfo::insertIndirectBranch!");
}

bool Comet2InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  // TODO RISCVのまま
  assert((Cond.size() == 3) && "Invalid branch condition!");
  Cond[0].setImm(getOppositeBranchOpcode(Cond[0].getImm()));
  return false;
}

MachineBasicBlock *
Comet2InstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
  assert(MI.getDesc().isBranch() && "Unexpected opcode!");
  // TODO RISCVのまま
  // The branch target is always the last operand.
  int NumOp = MI.getNumExplicitOperands();
  return MI.getOperand(NumOp - 1).getMBB();
}

bool Comet2InstrInfo::isBranchOffsetInRange(unsigned BranchOp,
                                           int64_t BrOffset) const {
  // ジャンプ可能な範囲内なら真を返す
  return isIntN(16, BrOffset);  // BrOffsetを符号付き16bitで表現できる場合、真
}

unsigned Comet2InstrInfo::getInstSizeInBytes(const MachineInstr &MI) const {
  unsigned Opcode = MI.getOpcode();

  switch (Opcode) {
  case Comet2::ADDAREG:
  case Comet2::SUBAREG:
  case Comet2::LDREG:
    return 1;
  default:
    return 2;
  }
}
