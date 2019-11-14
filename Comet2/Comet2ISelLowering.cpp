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


// NOTE 定義あり llvm/lib/CodeGen/TargetLoweringBase.cpp
#if 0
EVT Comet2TargetLowering::getSetCCResultType(const DataLayout &DL, LLVMContext &,
                                            EVT VT) const {
  // TODO RISCVのまま
  if (!VT.isVector())
    return getPointerTy(DL);
  return VT.changeVectorElementTypeToInteger();
}
#endif

// NOTE 定義あり llvm/lib/CodeGen/TargetLoweringBase.cpp
#if 0
bool Comet2TargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                                const AddrMode &AM, Type *Ty,
                                                unsigned AS,
                                                Instruction *I) const {
  // TODO RISCVのまま
  // No global is ever allowed as a base.
  if (AM.BaseGV)
    return false;

  // Require a 12-bit signed offset.
  if (!isInt<12>(AM.BaseOffs))
    return false;

  switch (AM.Scale) {
  case 0: // "r+i" or just "i", depending on HasBaseReg.
    break;
  case 1:
    if (!AM.HasBaseReg) // allow "r+i".
      break;
    return false; // disallow "r+r" or "r+r+i".
  default:
    return false;
  }

  return true;
}
#endif


// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::isLegalICmpImmediate(int64_t Imm) const {
  return isInt<12>(Imm);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::isLegalAddImmediate(int64_t Imm) const {
  return isInt<12>(Imm);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
// On RV32, 64-bit integers are split into their high and low parts and held
// in two different registers, so the trunc is free since the low register can
// just be used.
bool Comet2TargetLowering::isTruncateFree(Type *SrcTy, Type *DstTy) const {
  if (Subtarget.is64Bit() || !SrcTy->isIntegerTy() || !DstTy->isIntegerTy())
    return false;
  unsigned SrcBits = SrcTy->getPrimitiveSizeInBits();
  unsigned DestBits = DstTy->getPrimitiveSizeInBits();
  return (SrcBits == 64 && DestBits == 32);
}

bool Comet2TargetLowering::isTruncateFree(EVT SrcVT, EVT DstVT) const {
  if (Subtarget.is64Bit() || SrcVT.isVector() || DstVT.isVector() ||
      !SrcVT.isInteger() || !DstVT.isInteger())
    return false;
  unsigned SrcBits = SrcVT.getSizeInBits();
  unsigned DestBits = DstVT.getSizeInBits();
  return (SrcBits == 64 && DestBits == 32);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::isZExtFree(SDValue Val, EVT VT2) const {
  // Zexts are free if they can be combined with a load.
  if (auto *LD = dyn_cast<LoadSDNode>(Val)) {
    EVT MemVT = LD->getMemoryVT();
    if ((MemVT == MVT::i8 || MemVT == MVT::i16 ||
         (Subtarget.is64Bit() && MemVT == MVT::i32)) &&
        (LD->getExtensionType() == ISD::NON_EXTLOAD ||
         LD->getExtensionType() == ISD::ZEXTLOAD))
      return true;
  }

  return TargetLowering::isZExtFree(Val, VT2);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::isSExtCheaperThanZExt(EVT SrcVT, EVT DstVT) const {
  return Subtarget.is64Bit() && SrcVT == MVT::i32 && DstVT == MVT::i64;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::hasBitPreservingFPLogic(EVT VT) const {
  return false;
}
#endif

SDValue Comet2TargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {
  // Custum に指定したノードに対する操作
  report_fatal_error("unimplemented operand");
  return SDValue();
}

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
// customに登録されていて、戻り値がillegalのノードで呼び出される
void Comet2TargetLowering::ReplaceNodeResults(SDNode *N,
                                             SmallVectorImpl<SDValue> &Results,
                                             SelectionDAG &DAG) const {
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
SDValue Comet2TargetLowering::PerformDAGCombine(SDNode *N,
                                               DAGCombinerInfo &DCI) const {
  return SDValue();
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::isDesirableToCommuteWithShift(
    const SDNode *N, CombineLevel Level) const {
  return true;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
// 符号ビットに関する追加情報が必要な場合に実装する
unsigned Comet2TargetLowering::ComputeNumSignBitsForTargetNode(
    SDValue Op, const APInt &DemandedElts, const SelectionDAG &DAG,
    unsigned Depth) const {
  return 1;
}
#endif

// NOTE 宣言あり llvm/include/llvm/CodeGen/TargetLowering.h
MachineBasicBlock *
Comet2TargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                  MachineBasicBlock *BB) const {
  // TODO 実装不要？
  llvm_unreachable("Unexpected instr type to insert");
  return BB;
}

// TODO 不要？
// Calling Convention Implementation.
// The expectations for frontend ABI lowering vary from target to target.
// Ideally, an LLVM frontend would be able to avoid worrying about many ABI
// details, but this is a longer term goal. For now, we simply try to keep the
// role of the frontend as simple and well-defined as possible. The rules can
// be summarised as:
// * Never split up large scalar arguments. We handle them here.
// * If a hardfloat calling convention is being used, and the struct may be
// passed in a pair of registers (fp+fp, int+fp), and both registers are
// available, then pass as two separate arguments. If either the GPRs or FPRs
// are exhausted, then pass according to the rule below.
// * If a struct could never be passed in registers or directly in a stack
// slot (as it is larger than 2*XLEN and the floating point rules don't
// apply), then pass it using a pointer with the byval attribute.
// * If a struct is less than 2*XLEN, then coerce to either a two-element
// word-sized array or a 2*XLEN scalar (depending on alignment).
// * The frontend can determine whether a struct is returned by reference or
// not based on its size and fields. If it will be returned by reference, the
// frontend must modify the prototype so a pointer with the sret annotation is
// passed as the first argument. This is not necessary for large scalar
// returns.
// * Struct return values and varargs should be coerced to structs containing
// register-size fields in the same situations they would be for fixed
// arguments.


// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
// Transform physical registers into virtual registers.
SDValue Comet2TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

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

  return Chain;
}


// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
  return true;
}
#endif

SDValue
Comet2TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  // TODO RISCVからの抜粋

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

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
#if 0
// ノード名を返す
const char *Comet2TargetLowering::getTargetNodeName(unsigned Opcode) const {
  return nullptr;
}
#endif

// NOTE 定義あり llvm/lib/CodeGen/SelectionDAG/TargetLowering.cpp
#if 0
/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
Comet2TargetLowering::ConstraintType
Comet2TargetLowering::getConstraintType(StringRef Constraint) const {
  return TargetLowering::getConstraintType(Constraint);
}
#endif

// NOTE 定義あり llvm/lib/CodeGen/SelectionDAG/TargetLowering.cpp
#if 0
std::pair<unsigned, const TargetRegisterClass *>
Comet2TargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                  StringRef Constraint,
                                                  MVT VT) const {
  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
unsigned
Comet2TargetLowering::getInlineAsmMemConstraint(StringRef ConstraintCode) const {
  return TargetLowering::getInlineAsmMemConstraint(ConstraintCode);
}
#endif

// NOTE 定義あり llvm/lib/CodeGen/SelectionDAG/TargetLowering.cpp
#if 0
void Comet2TargetLowering::LowerAsmOperandForConstraint(
    SDValue Op, std::string &Constraint, std::vector<SDValue> &Ops,
    SelectionDAG &DAG) const {
  TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
Instruction *Comet2TargetLowering::emitLeadingFence(IRBuilder<> &Builder,
                                                   Instruction *Inst,
                                                   AtomicOrdering Ord) const {
  return nullptr;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
Instruction *Comet2TargetLowering::emitTrailingFence(IRBuilder<> &Builder,
                                                    Instruction *Inst,
                                                    AtomicOrdering Ord) const {
  return nullptr;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
TargetLowering::AtomicExpansionKind
Comet2TargetLowering::shouldExpandAtomicRMWInIR(AtomicRMWInst *AI) const {
  return AtomicExpansionKind::None;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
Value *Comet2TargetLowering::emitMaskedAtomicRMWIntrinsic(
    IRBuilder<> &Builder, AtomicRMWInst *AI, Value *AlignedAddr, Value *Incr,
    Value *Mask, Value *ShiftAmt, AtomicOrdering Ord) const {
  return nullptr;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
TargetLowering::AtomicExpansionKind
Comet2TargetLowering::shouldExpandAtomicCmpXchgInIR(
    AtomicCmpXchgInst *CI) const {
  return AtomicExpansionKind::None;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
Value *Comet2TargetLowering::emitMaskedAtomicCmpXchgIntrinsic(
    IRBuilder<> &Builder, AtomicCmpXchgInst *CI, Value *AlignedAddr,
    Value *CmpVal, Value *NewVal, Value *Mask, AtomicOrdering Ord) const {
  return nullptr;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
unsigned Comet2TargetLowering::getExceptionPointerRegister(
    const Constant *PersonalityFn) const {
  return 0;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
unsigned Comet2TargetLowering::getExceptionSelectorRegister(
    const Constant *PersonalityFn) const {
  return 0;
}
#endif

// NOTE 定義あり llvm/include/llvm/CodeGen/TargetLowering.h
#if 0
bool Comet2TargetLowering::shouldExtendTypeInLibCall(EVT Type) const {
  return true;
}
#endif
