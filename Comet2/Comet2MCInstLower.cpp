//===-- Comet2MCInstLower.cpp - Convert COMET-II MachineInstr to an MCInst --=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower COMET-II MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "Comet2.h"
#include "MCTargetDesc/Comet2MCExpr.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "comet2-mcinstlower"

static MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym,
                                    const AsmPrinter &AP) {
  // TODO ほぼRISCV
  MCContext &Ctx = AP.OutContext;

  const MCExpr *ME =
      MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    ME = MCBinaryExpr::createAdd(
        ME, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  return MCOperand::createExpr(ME);
}

bool llvm::LowerComet2MachineOperandToMCOperand(const MachineOperand &MO,
                                               MCOperand &MCOp,
                                               const AsmPrinter &AP) {
  LLVM_DEBUG(dbgs() << "### LowerComet2MachineOperandToMCOperand 0\n");

  // TODO ほぼRISCV あまりターゲット依存がない？
  switch (MO.getType()) {
  default:
    report_fatal_error("LowerComet2MachineInstrToMCInst: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  case MachineOperand::MO_MachineBasicBlock:
    MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), AP);
    break;
  case MachineOperand::MO_GlobalAddress:
    MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
    break;
  case MachineOperand::MO_BlockAddress:
    MCOp = lowerSymbolOperand(
        MO, AP.GetBlockAddressSymbol(MO.getBlockAddress()), AP);
    break;
  case MachineOperand::MO_ExternalSymbol:
    MCOp = lowerSymbolOperand(
        MO, AP.GetExternalSymbolSymbol(MO.getSymbolName()), AP);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    MCOp = lowerSymbolOperand(MO, AP.GetCPISymbol(MO.getIndex()), AP);
    break;
  }
  return true;
}

void llvm::LowerComet2MachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                          const AsmPrinter &AP) {
  LLVM_DEBUG(dbgs() << "### LowerComet2MachineInstrToMCInst " << *MI << "\n");
  // TODO ほぼRISCV あまりターゲット依存がない？
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (LowerComet2MachineOperandToMCOperand(MO, MCOp, AP))
      OutMI.addOperand(MCOp);
  }
}
