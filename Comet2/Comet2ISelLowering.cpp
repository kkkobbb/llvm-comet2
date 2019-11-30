//===-- Comet2ISelLowering.cpp - COMET-II DAG Lowering Implementation  ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that COMET-II uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Comet2ISelLowering.h"
#include "Comet2.h"
#include "Comet2MachineFunctionInfo.h"
#include "Comet2RegisterInfo.h"
#include "Comet2Subtarget.h"
#include "Comet2TargetMachine.h"
#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#include "Comet2GenCallingConv.inc"

#define DEBUG_TYPE "comet2-lower"

STATISTIC(NumTailCalls, "Number of tail calls");

Comet2TargetLowering::Comet2TargetLowering(const TargetMachine &TM,
                                           const Comet2Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i16, &Comet2::IntRegsRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(Comet2::GR7);

  setOperationAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setOperationAction(ISD::EXTLOAD,  MVT::i8,  Promote);
  setOperationAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setOperationAction(ISD::ZEXTLOAD, MVT::i8,  Promote);
  setOperationAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setOperationAction(ISD::SEXTLOAD, MVT::i8,  Promote);

  // TODO 命令のアラインメント？
  setMinFunctionAlignment(llvm::Align(2));

  // TODO gcc での __atomic_* 関数対応かどうか？
  setMaxAtomicSizeInBitsSupported(0);

  // booleanの表現型
  setBooleanContents(ZeroOrOneBooleanContent);
}

SDValue Comet2TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  // Custum に指定したノードに対する操作
  report_fatal_error("unimplemented operand");
  return SDValue();
}

// NOTE 宣言あり llvm/include/llvm/CodeGen/TargetLowering.h
MachineBasicBlock *
Comet2TargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                  MachineBasicBlock *BB) const {
  // TODO 実装不要？
  llvm_unreachable("Unexpected instr type to insert");
  return BB;
}

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
// Transform physical registers into virtual registers.
SDValue Comet2TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  LLVM_DEBUG(dbgs() << "### LowerFormalArguments\n");

  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::C:
  case CallingConv::Fast:
    break;
  }

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  const Function &Func = MF.getFunction();
  if (Func.hasFnAttribute("interrupt")) {
    if (!Func.arg_empty())
      report_fatal_error(
        "Functions with the interrupt attribute cannot have arguments!");

    StringRef Kind =
      MF.getFunction().getFnAttribute("interrupt").getValueAsString();

    if (!(Kind == "user" || Kind == "supervisor" || Kind == "machine"))
      report_fatal_error(
        "Function interrupt attribute argument not supported!");
  }

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Comet2);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      LLVM_DEBUG(dbgs() << "### LowerFormalArguments Reg\n");
      // 引数がレジスタ経由で渡された場合
      EVT RegVT = VA.getLocVT();
      // 使用するレジスタクラスを指定
      const TargetRegisterClass *RC = &Comet2::IntRegsArgsRegClass;
      if (VA.getLocInfo() != CCValAssign::Full) {
        llvm_unreachable("not supported yet");
      }

      // 仮想レジスタを作成
      Register VReg = RegInfo.createVirtualRegister(RC);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);
      InVals.push_back(ArgValue);
    } else {
      LLVM_DEBUG(dbgs() << "### LowerFormalArguments Mem\n");
      // 引数がスタック経由で渡された場合
      assert(VA.isMemLoc());

      unsigned ObjSize = VA.getLocVT().getSizeInBits() / 8;
      int FI = MFI.CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);

      // スタックから引数を取得するためにloadノードを作成する
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
      InVals.push_back(DAG.getLoad(VA.getLocVT(), DL, Chain, FIN,
                  MachinePointerInfo::getFixedStack(MF, FI)));
    }
  }

  if (IsVarArg) {
      report_fatal_error(
        "VarArg not supported!");
  }

  return Chain;
}

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
// Lower a call to a callseq_start + CALL + callseq_end chain, and add input
// and output parameter nodes.
SDValue Comet2TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  LLVM_DEBUG(dbgs() << "### LowerCall\n");

  SelectionDAG &DAG = CLI.DAG;
  //SDLoc &DL = CLI.DL;
  //SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  //SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  //SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  //bool &IsTailCall = CLI.IsTailCall;
  //CallingConv::ID CallConv = CLI.CallConv;
  //bool IsVarArg = CLI.IsVarArg;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  //MVT XLenVT = Subtarget.getXLenVT();

  //MachineFunction &MF = DAG.getMachineFunction();

  // TODO 未実装

  // FIXME ここがおかしい？

  return Chain;
}

SDValue
Comet2TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  LLVM_DEBUG(dbgs() << "### LowerReturn\n");
  LLVM_DEBUG(Chain->dumpr());

  // TODO RISCVからの抜粋

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_Comet2);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    // Handle a 'normal' return.
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

    // Guarantee that all emitted copies are stuck together.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the glue node if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(Comet2ISD::Ret, DL, MVT::Other, RetOps);
}

// NOTE 定義あり llvm/lib/CodeGen/SelectionDAG/TargetLowering.cpp
// ノード名を返す
const char *Comet2TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((Comet2ISD::NodeType)Opcode) {
  case Comet2ISD::FIRST_NUMBER:
    break;
  case Comet2ISD::Call:
    return "Comet2ISD::Call";
  case Comet2ISD::Ret:
    return "Comet2ISD::Ret";
  }
  return nullptr;
}
