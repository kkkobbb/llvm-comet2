//===-- Comet2InstPrinter.cpp - Convert Comet2 MCInst to asm syntax -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an Comet2 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Comet2InstPrinter.h"
#include "MCTargetDesc/Comet2MCExpr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "Comet2GenAsmWriter.inc"

void Comet2InstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                 StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void Comet2InstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
  O << getRegisterName(RegNo);
}

void Comet2InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");

  LLVM_DEBUG(dbgs() << "### printOperand " << *MI << " " << OpNo << "\n");

  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  if (MO.isImm()) {
    O << MO.getImm();
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

void Comet2InstPrinter::printMemOperand(const MCInst *MI, unsigned OpNo,
                                        raw_ostream &O) {
  LLVM_DEBUG(dbgs() << "### printMemOperand:"; MI->dump());
  // TODO Comet2用表示
  printOperand(MI, OpNo + 1, O);
  O << "(";
  printOperand(MI, OpNo, O);
  O << ")";
}
