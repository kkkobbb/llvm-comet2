//===-- Comet2ISelDAGToDAG.cpp - A dag to dag inst selector for COMET-II --===//
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Comet-II target.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "Comet2.h"
#include "Comet2TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "comet2-isel"

// Comet2-specific code to select Comet2 machine instructions for
// SelectionDAG operations.
namespace {
class Comet2DAGToDAGISel final : public SelectionDAGISel {
  const Comet2Subtarget *Subtarget;

public:
  explicit Comet2DAGToDAGISel(Comet2TargetMachine &TargetMachine)
      : SelectionDAGISel(TargetMachine) {}

  StringRef getPassName() const override {
    return "Comet2 DAG->DAG Pattern Instruction Selection";
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    Subtarget = &MF.getSubtarget<Comet2Subtarget>();
    return SelectionDAGISel::runOnMachineFunction(MF);
  }

  void Select(SDNode *Node) override;

  // Complex Pattern.
  bool SelectAddr(SDValue Addr, SDValue &Base);

// Include the pieces autogenerated from the target description.
#include "Comet2GenDAGISel.inc"
};
}

void Comet2DAGToDAGISel::Select(SDNode *Node) {
  // Select the default instruction.
  SelectCode(Node);
}

bool Comet2DAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Base) {
  // TODO RISCVのまま
  if (auto FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), Subtarget->getXLenVT());
    return true;
  }
  return false;
}

// This pass converts a legalized DAG into a Comet2-specific DAG, ready
// for instruction scheduling.
FunctionPass *llvm::createComet2ISelDag(Comet2TargetMachine &TM) {
  return new Comet2DAGToDAGISel(TM);
}
