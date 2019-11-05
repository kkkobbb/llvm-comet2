//===-- Comet2AsmPrinter.cpp - COMET-II LLVM assembly writer --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the COMET-II assembly language.
//
//===----------------------------------------------------------------------===//

#include "Comet2.h"
#include "MCTargetDesc/Comet2InstPrinter.h"
#include "MCTargetDesc/Comet2MCExpr.h"
#include "Comet2TargetMachine.h"
#include "TargetInfo/Comet2TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class Comet2AsmPrinter : public AsmPrinter {
public:
  explicit Comet2AsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Comet2 Assembly Printer"; }

  void EmitInstruction(const MachineInstr *MI) override;

#if 0
  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

  void EmitToStreamer(MCStreamer &S, const MCInst &Inst);
#endif
};
}

// NOTE 定義あり lib/CodeGen/AsmPrinter/AsmPrinter.cpp
#if 0
void Comet2AsmPrinter::EmitToStreamer(MCStreamer &S, const MCInst &Inst) {
}
#endif

void Comet2AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  // TODO 実装はこれでいい？
  MCInst TmpInst;
  LowerComet2MachineInstrToMCInst(MI, TmpInst, *this);
  EmitToStreamer(*OutStreamer, TmpInst);
}

// NOTE 定義あり llvm/lib/CodeGen/AsmPrinter/AsmPrinterInlineAsm.cpp
#if 0
// インラインアセンブラ用
bool Comet2AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                      const char *ExtraCode, raw_ostream &OS) {
}
#endif

// NOTE 定義あり llvm/lib/CodeGen/AsmPrinter/AsmPrinterInlineAsm.cpp
#if 0
// インラインアセンブラ用
bool Comet2AsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                            unsigned OpNo,
                                            const char *ExtraCode,
                                            raw_ostream &OS) {
}
#endif

// Force static initialization.
extern "C" void LLVMInitializeComet2AsmPrinter() {
  RegisterAsmPrinter<Comet2AsmPrinter> X(getTheComet2Target());
}
