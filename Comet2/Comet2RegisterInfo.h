//===-- Comet2RegisterInfo.h - COMET-II Register Information Impl ---*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file contains the COMET-II implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2REGISTERINFO_H
#define LLVM_LIB_TARGET_COMET2_COMET2REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "Comet2GenRegisterInfo.inc"

namespace llvm {

struct Comet2RegisterInfo : public Comet2GenRegisterInfo {

  Comet2RegisterInfo(unsigned HwMode);

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};
}

#endif
