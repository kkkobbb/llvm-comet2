//===-- Comet2.td - Describe the COMET II Target Machine ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// COMET-II subtarget features and instruction predicates.
//===----------------------------------------------------------------------===//

// nothing

//===----------------------------------------------------------------------===//
// Registers, calling conventions, instruction descriptions.
//===----------------------------------------------------------------------===//

include "Comet2RegisterInfo.td"
include "Comet2CallingConv.td"
include "Comet2InstrInfo.td"

//===----------------------------------------------------------------------===//
// COMET-II processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"Comet2", NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the COMET-II target.
//===----------------------------------------------------------------------===//

def Comet2InstrInfo : InstrInfo {
  // NOTE TableGenが未設定のプロパティを推測するか (将来廃止される)
  let guessInstructionProperties = 0;
}

// TODO Target.tdに定義されているDefaultAsmWriterでもいい？
def Comet2AsmWriter : AsmWriter;

def Comet2 : Target {
  let InstructionSet = Comet2InstrInfo;
  let AssemblyWriters = [Comet2AsmWriter];
  // NOTE レジスタ割り当て後のレジスタ名変更をしてもいいか
  // NOTE (MachineCopyPropagation最適化を適用するかに影響してるっぽい) <https://reviews.llvm.org/D43042>
  let AllowRegisterRenaming = 1;
}
