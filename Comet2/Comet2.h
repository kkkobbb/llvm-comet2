//===-- Comet2.h - Top-level interface for COMET-II -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// COMET-II back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2_H
#define LLVM_LIB_TARGET_COMET2_COMET2_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class Comet2Subtarget;
class Comet2TargetMachine;
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

void LowerComet2MachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                     const AsmPrinter &AP);
bool LowerComet2MachineOperandToMCOperand(const MachineOperand &MO,
                                          MCOperand &MCOp, const AsmPrinter &AP);

FunctionPass *createComet2ISelDag(Comet2TargetMachine &TM);
}

#endif
