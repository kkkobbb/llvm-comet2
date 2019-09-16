//===-- Comet2.td - Describe the COMET II Target Machine ---*- tablegen -*-===//
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// COMET-II subtarget features and instruction predicates.
//===----------------------------------------------------------------------===//

// TODO 定義不要？
// nothing

//===----------------------------------------------------------------------===//
// Named operands for CSR instructions.
//===----------------------------------------------------------------------===//

// NOTE CSR (Control/Status Register)

include "Comet2SystemOperands.td"

//===----------------------------------------------------------------------===//
// Registers, calling conventions, instruction descriptions.
//===----------------------------------------------------------------------===//

include "Comet2RegisterInfo.td"
include "Comet2CallingConv.td"
include "Comet2InstrInfo.td"
include "Comet2RegisterBanks.td"

//===----------------------------------------------------------------------===//
// COMET-II processors supported.
//===----------------------------------------------------------------------===//

// TODO subtargetがない場合、定義は不要？
// nothing

//===----------------------------------------------------------------------===//
// Define the COMET-II target.
//===----------------------------------------------------------------------===//

def Comet2InstrInfo : InstrInfo {
  // NOTE TableGenが未設定のプロパティを推測するか (将来廃止される)
  let guessInstructionProperties = 0;
}

def Comet2AsmParser : AsmParser {
  // NOTE レジスタ別名解決処理が必要か
  let ShouldEmitMatchRegisterAltName = 1;
  // NOTE レジスタ名とレジスタ別名が重複してもいいか？
  let AllowDuplicateRegisterNames = 1;
}

def Comet2AsmWriter : AsmWriter {
  // MCSubtargetInfoを様々な print methodに渡すかどうか？ (将来廃止される)
  int PassSubtarget = 1;
}

def Comet2 : Target {
  let InstructionSet = Comet2InstrInfo;
  let AssemblyParsers = [Comet2AsmParser];
  let AssemblyWriters = [Comet2AsmWriter];
  // NOTE レジスタ割り当て後のレジスタ名変更をしてもいいか
  let AllowRegisterRenaming = 1;
}
