//===-- Comet2Disassembler.cpp - Disassembler for Comet2 ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Comet2Disassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "TargetInfo/Comet2TargetInfo.h"
#include "Utils/Comet2BaseInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "comet2-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
// NOTE llvm/include/llvm/MC/MCDisassembler/MCDisassembler.h
// NOTE llvm/lib/MC/MCDisassembler/MCDisassembler.cpp
class Comet2Disassembler : public MCDisassembler {

public:
  Comet2Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createComet2Disassembler(const Target &T,
                                                const MCSubtargetInfo &STI,
                                                MCContext &Ctx) {
  return new Comet2Disassembler(STI, Ctx);
}

extern "C" void LLVMInitializeComet2Disassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(getTheComet2Target(),
                                         createComet2Disassembler);
}

static const Register IntRegsDecoderTable[] = {
  Comet2::GR0,  Comet2::GR1,  Comet2::GR2,  Comet2::GR3,
  Comet2::GR4,  Comet2::GR5,  Comet2::GR6,  Comet2::GR7
};

static DecodeStatus DecodeIntRegsRegisterClass(MCInst &Inst, uint64_t RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  if (RegNo > array_lengthof(IntRegsDecoderTable))
    return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  Register Reg = IntRegsDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

#include "Comet2GenDisassemblerTables.inc"

static DecodeStatus DecodeMem(MCInst &Inst, uint64_t RegNo,
                              uint64_t Address,
                              const void *Decoder) {
    // TODO load/store命令のoperandをデコード
    return MCDisassembler::Success;
}

static DecodeStatus DecodeCallTarget(MCInst &Inst, uint64_t RegNo,
                                     uint64_t Address,
                                     const void *Decoder) {
    // TODO call命令のoperandをデコード
    return MCDisassembler::Success;
}

DecodeStatus Comet2Disassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                                ArrayRef<uint8_t> Bytes,
                                                uint64_t Address,
                                                raw_ostream &OS,
                                                raw_ostream &CS) const {
  uint32_t Insn;
  DecodeStatus Result;

  Insn = support::endian::read16le(Bytes.data());
  // TODO DecoderTable 名は正しい？
  Result = decodeInstruction(DecoderTableComet216, MI, Insn, Address, this, STI);
  Size = 2;

  return Result;
}
