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

// NOTE llvm/include/llvm/CodeGen/TargetFrameLowering.h hasFP
// 専用のフレームポインタレジスタを持つべき関数の場合、真を返す
// 可変サイズの"alloca"が含まれる場合などで真になる
bool Comet2FrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

void Comet2FrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  // ここでできることはない
  //   Comet2にはSPを直接読み書きする手段がないため、スタックはpush、popのみでしか変更できない
  // スタックは呼び出し先が責任をもって、関数終了時に呼び出し時の状態に戻す必要がある
  // emitEpilogueも同様
  return;
}

void Comet2FrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  return;
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
