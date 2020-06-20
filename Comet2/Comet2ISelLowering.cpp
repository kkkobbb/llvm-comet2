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

#define DEBUG_TYPE "comet2-isellowering"

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "Comet2GenCallingConv.inc"

Comet2TargetLowering::Comet2TargetLowering(const TargetMachine &TM,
                                           const Comet2Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i16, &Comet2::IntRegsRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(Comet2::GR7);

  // NOTE Promote 型を大きな型として扱う
  setOperationAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setOperationAction(ISD::EXTLOAD,  MVT::i8,  Promote);
  setOperationAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setOperationAction(ISD::ZEXTLOAD, MVT::i8,  Promote);
  setOperationAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setOperationAction(ISD::SEXTLOAD, MVT::i8,  Promote);

  // NOTE Custom LowerOperation()に渡す
  setOperationAction(ISD::BR_CC, MVT::i16, Custom);

  // NOTE ISD::SETLE等の条件の場合、setCondCodeAction()を使う

  // NOTE llvm/include/llvm/CodeGen/TargetLowering.h setMaxAtomicSizeInBitsSupported
  // NOTE バックエンドがサポートする最大のアトミック操作のサイズ
  // これより大きい場合、__atomic_*ライブラリに展開される
  setMaxAtomicSizeInBitsSupported(0);

  // booleanの表現型
  setBooleanContents(ZeroOrOneBooleanContent);
}

SDValue Comet2TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  // Custom に指定したノードに対する操作
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  case ISD::BR_CC:
    return lowerBB_CC(Op, DAG);
  }
  return SDValue();
}

SDValue Comet2TargetLowering::lowerBB_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS   = Op.getOperand(2);
  SDValue RHS   = Op.getOperand(3);
  SDValue Dest  = Op.getOperand(4);
  SDLoc dl  (Op);

  Comet2ISD::NodeType compNT;
  Comet2ISD::NodeType jumpNT;
  switch (CC) {
  default:
    llvm_unreachable("unimplemented condition");
  case ISD::SETEQ:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JZE;
    break;
  case ISD::SETUEQ:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JZE;
    break;
  case ISD::SETNE:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JNZ;
    break;
  case ISD::SETUNE:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JNZ;
    break;
  case ISD::SETLT:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JPL;
    break;
  case ISD::SETULT:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JPL;
    break;
  case ISD::SETGT:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JMI;
    break;
  case ISD::SETUGT:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JMI;
    break;
  case ISD::SETLE:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JLE;
    break;
  case ISD::SETULE:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JLE;
    break;
  case ISD::SETGE:
    compNT = Comet2ISD::CPA;
    jumpNT = Comet2ISD::JGE;
    break;
  case ISD::SETUGE:
    compNT = Comet2ISD::CPL;
    jumpNT = Comet2ISD::JGE;
    break;
  }

  auto compNode = DAG.getNode(compNT, dl, MVT::Glue, LHS, RHS);
  return DAG.getNode(jumpNT, dl, Op.getValueType(), Chain, Dest, compNode);
}

// NOTE llvm/include/llvm/CodeGen/TargetLowering.h EmitInstrWithCustomInserter
// NOTE *.td等でusesCustomInserterを設定した命令がある場合この関数が呼び出される
MachineBasicBlock *
Comet2TargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                  MachineBasicBlock *BB) const {
  // Comet2では不要
  llvm_unreachable("Unexpected instr type to insert");
}

// NOTE llvm/include/llvm/CodeGen/TargetLowering.h LowerFormalArguments
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
      const TargetRegisterClass *RC = &Comet2::IntRegsRegClass;
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
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i16);
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

// NOTE llvm/include/llvm/CodeGen/TargetLowering.h LowerCall
// Lower a call to a callseq_start + CALL + callseq_end chain, and add input
// and output parameter nodes.
SDValue Comet2TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  LLVM_DEBUG(dbgs() << "### LowerCall\n");

  // NOTE AVR参考
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &isTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool isVarArg = CLI.IsVarArg;

  // Comet2 does not yet support tail call optimization.
  isTailCall = false;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol
  // node so that legalize doesn't hack it.
  if (const GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = G->getGlobal();

    Callee =
        DAG.getTargetGlobalAddress(GV, DL, getPointerTy(DAG.getDataLayout()));
  } else if (const ExternalSymbolSDNode *ES =
                 dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(ES->getSymbol(),
                                         getPointerTy(DAG.getDataLayout()));
  }

  // NOTE 関数呼び出し時の引数の処理
  CCInfo.AnalyzeCallOperands(Outs, CC_Comet2);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, DL);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;

  // First, walk the register assignments, inserting copies.
  unsigned AI, AE;
  bool HasStackArgs = false;
  for (AI = 0, AE = ArgLocs.size(); AI != AE; ++AI) {
    CCValAssign &VA = ArgLocs[AI];
    EVT RegVT = VA.getLocVT();
    SDValue Arg = OutVals[AI];

    // Promote the value if needed. With Clang this should not happen.
    switch (VA.getLocInfo()) {
    default:
      llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, DL, RegVT, Arg);
      break;
    case CCValAssign::BCvt:
      Arg = DAG.getNode(ISD::BITCAST, DL, RegVT, Arg);
      break;
    }

    // Stop when we encounter a stack argument, we need to process them
    // in reverse order in the loop below.
    if (VA.isMemLoc()) {
      HasStackArgs = true;
      break;
    }

    // Arguments that can be passed on registers must be kept in the RegsToPass
    // vector.
    RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
  }

  // Second, stack arguments have to walked in reverse order by inserting
  // chained stores, this ensures their order is not changed by the scheduler
  // and that the push instruction sequence generated is correct, otherwise they
  // can be freely intermixed.
  if (HasStackArgs) {
    LLVM_DEBUG(dbgs() << "### LowerCall HasStack\n");
    for (AE = AI, AI = ArgLocs.size(); AI != AE; --AI) {
      unsigned Loc = AI - 1;
      CCValAssign &VA = ArgLocs[Loc];
      SDValue Arg = OutVals[Loc];

      assert(VA.isMemLoc());

      // スタックに引数を格納する
      // ここで(store reg1, (add reg2, offset))のようなノードを作るとうまく変換してくれないため
      // 専用のノードSTREGOFFSETを生成している
      // NOTE Comet2InstrInfo.tdの定義でSDNPHasChainを指定しているのでChainを渡す必要がある
      Chain = DAG.getNode(Comet2ISD::STREGOFFSET, DL, getPointerTy(DAG.getDataLayout()),
          Chain, Arg,
          DAG.getRegister(Comet2::GR7, getPointerTy(DAG.getDataLayout())),
          DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));
    }
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain and
  // flag operands which copy the outgoing args into registers.  The InFlag in
  // necessary since all emited instructions must be stuck together.
  SDValue InFlag;
  for (auto Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (auto Reg : RegsToPass) {
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  }

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask =
      TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode()) {
    Ops.push_back(InFlag);
  }

  Chain = DAG.getNode(Comet2ISD::CALL, DL, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, DL, true),
                             DAG.getIntPtrConstant(0, DL, true), InFlag, DL);

  if (!Ins.empty()) {
    InFlag = Chain.getValue(1);
  }

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, DL, DAG,
                         InVals);
}

/// Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
///
SDValue Comet2TargetLowering::LowerCallResult(
    SDValue Chain, SDValue InFlag, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Handle runtime calling convs.
  CCInfo.AnalyzeCallResult(Ins, RetCC_Comet2);

  // Copy all of the result registers out of their specified physreg.
  for (CCValAssign const &RVLoc : RVLocs) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLoc.getLocReg(), RVLoc.getValVT(),
                               InFlag)
                .getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

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

  // NOTE RISCV参考

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

  return DAG.getNode(Comet2ISD::RET, DL, MVT::Other, RetOps);
}

// NOTE llvm/lib/CodeGen/SelectionDAG/TargetLowering.cpp getTargetNodeName
// ノード名を返す
const char *Comet2TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((Comet2ISD::NodeType)Opcode) {
  case Comet2ISD::FIRST_NUMBER:
    break;
  case Comet2ISD::CALL:
    return "Comet2ISD::CALL";
  case Comet2ISD::RET:
    return "Comet2ISD::RET";
  case Comet2ISD::STREGOFFSET:
    return "Comet2ISD::STREGOFFSET";
  case Comet2ISD::CPA:
    return "Comet2ISD::CPA";
  case Comet2ISD::CPL:
    return "Comet2ISD::CPL";
  case Comet2ISD::JZE:
    return "Comet2ISD::JZE";
  case Comet2ISD::JNZ:
    return "Comet2ISD::JNZ";
  case Comet2ISD::JPL:
    return "Comet2ISD::JPL";
  case Comet2ISD::JMI:
    return "Comet2ISD::JMI";
  case Comet2ISD::JLE:
    return "Comet2ISD::JLE";
  case Comet2ISD::JGE:
    return "Comet2ISD::JGE";
  }
  return nullptr;
}

