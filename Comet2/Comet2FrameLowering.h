//===-- Comet2FrameLowering.h - Define frame lowering for COMET-II -*- C++ -*--===//
//
//===----------------------------------------------------------------------===//
//
// This class implements COMET-II-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2FRAMELOWERING_H
#define LLVM_LIB_TARGET_COMET2_COMET2FRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class Comet2Subtarget;

class Comet2FrameLowering : public TargetFrameLowering {
public:
  explicit Comet2FrameLowering(const Comet2Subtarget &STI)
      : TargetFrameLowering(StackGrowsDown,
                            /*StackAlignment=*/16,
                            /*LocalAreaOffset=*/0),
        STI(STI) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  bool hasFP(const MachineFunction &MF) const override;

#if 0
  int getFrameIndexReference(const MachineFunction &MF, int FI,
                             unsigned &FrameReg) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  void processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                           RegScavenger *RS) const override;

  bool hasReservedCallFrame(const MachineFunction &MF) const override;
  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override;
#endif

protected:
  const Comet2Subtarget &STI;
};
}
#endif
