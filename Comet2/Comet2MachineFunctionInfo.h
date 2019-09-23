//=- Comet2VMachineFunctionInfo.h - COMET-II machine function info -*- C++ -*-=//
//
//===----------------------------------------------------------------------===//
//
// This file declares COMET-II-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2MACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_COMET2_COMET2MACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class Comet2MachineFunctionInfo : public MachineFunctionInfo {
public:
  Comet2MachineFunctionInfo(MachineFunction &MF) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_COMET2_COMET2MACHINEFUNCTIONINFO_H
