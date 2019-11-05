//===-- Comet2MCExpr.cpp - COMET-II specific MC expression classes --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the COMET-II architecture
//
//===----------------------------------------------------------------------===//

#include "Comet2MCExpr.h"
#include "MCTargetDesc/Comet2AsmBackend.h"
#include "Comet2.h"
#include "Comet2FixupKinds.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "comet2mcexpr"

// NOTE llvm/include/llvm/MC/MCExpr.h
// NOTE llvm/lib/MC/MCExpr.cpp

const Comet2MCExpr *Comet2MCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                         MCContext &Ctx) {
  return new (Ctx) Comet2MCExpr(Expr, Kind);
}

// TODO 定義必要
void Comet2MCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  // TODO 追加が必要？
  Expr->print(OS, MAI);
}

// TODO 定義必要
bool Comet2MCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                             const MCAsmLayout *Layout,
                                             const MCFixup *Fixup) const {
  // TODO 追加が必要？
  return getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup);
}

// TODO 定義必要
void Comet2MCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  // TODO 追加が必要？
  Streamer.visitUsedExpr(*getSubExpr());
}

// TODO 定義不要
#if 0
Comet2MCExpr::VariantKind Comet2MCExpr::getVariantKindForName(StringRef name) {
  return StringSwitch<Comet2MCExpr::VariantKind>(name)
      .Default(VK_RISCV_Invalid);
}
#endif

// TODO 定義不要
#if 0
StringRef Comet2MCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  }
}
#endif

// TODO 定義必要
void Comet2MCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  // TODO 追加が必要？
}
