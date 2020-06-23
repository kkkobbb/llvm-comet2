//===-- Comet2InstrInfo.td - Target Description for COMET II -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the COMET-II instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// COMET II specific DAG Nodes.
//===----------------------------------------------------------------------===//

// NOTE llvm/include/llvm/CodeGen/SDNodeProperties.td SDNP*

// NOTE SDTypeProfile<出力の数, 入力の数, [入力の制約, ...]>
def SDT_Comet2Call : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def Comet2Call : SDNode<"Comet2ISD::CALL", SDT_Comet2Call,
                        [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                         SDNPVariadic]>;

def Comet2Ret : SDNode<"Comet2ISD::RET", SDTNone,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_Comet2St : SDTypeProfile<0, 3, []>;
def Comet2StRegOffset : SDNode<"Comet2ISD::STREGOFFSET", SDT_Comet2St,
                               [SDNPHasChain, SDNPInGlue, SDNPMayStore]>;

def SDT_Comet2Comp : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def Comet2Cpa : SDNode<"Comet2ISD::CPA", SDT_Comet2Comp, [SDNPOutGlue]>;
def Comet2Cpl : SDNode<"Comet2ISD::CPL", SDT_Comet2Comp, [SDNPOutGlue]>;

def SDT_Comet2Br : SDTypeProfile<0, 1, [SDTCisVT<0, OtherVT>]>;
def Comet2Jze : SDNode<"Comet2ISD::JZE", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;
def Comet2Jnz : SDNode<"Comet2ISD::JNZ", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;
def Comet2Jpl : SDNode<"Comet2ISD::JPL", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;
def Comet2Jmi : SDNode<"Comet2ISD::JMI", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;
def Comet2Jle : SDNode<"Comet2ISD::JLE", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;
def Comet2Jge : SDNode<"Comet2ISD::JGE", SDT_Comet2Br, [SDNPHasChain, SDNPInGlue]>;

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


//===----------------------------------------------------------------------===//
// Operand and SDNode transformation definitions.
//===----------------------------------------------------------------------===//

// アドレス (call用)
def calltarget : Operand<iPTR> {
  let EncoderMethod = "getAddrTargetOpValue";
}

// アドレス (番地 + レジスタ)
def mem : Operand<i16> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops IntRegs, i16imm);
  let EncoderMethod = "getMemEncoding";
}

// アドレス (BasicBlock用)
def brtarget : Operand<OtherVT> {
  let EncoderMethod = "getBrTargetOpValue";
}

// 符号付き16bit整数
// 定数(ISD::Constant)で16bitで表現可能なもの
def immSExt16 : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

// NOTE llvm/include/llvm/CodeGen/SDNodeProperties.td SDNPWantRoot
// NOTE llvm/include/llvm/Target/TargetSelectionDAG.td ComplexPattern
// NOTE ComplexPattern<パターンの型, オペランドの数, 生成時の関数名, マッチできるルートノード, 追加の設定>
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], [SDNPWantRoot]>;


//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//

include "Comet2InstrFormats.td"


//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

// 算術命令 register,register
// RegisterClassを引数にするのは本来不要だが、このクラスだけ練習用として記述している
// NOTE 入力と出力が同じレジスタの場合の命令は AVR ADD命令が参考になるかも
class Comet2InstRegRegSet<bits<8> op, string asmstr, SDNode OpNode, RegisterClass RC>
    : Comet2InstRegReg<op, (outs RC:$r1), (ins RC:$src, RC:$r2),
                       !strconcat(asmstr, " $r1,$r2"),
                       [(set i16:$r1, (OpNode i16:$src, i16:$r2))]> {
  // NOTE Defs オペランドに現れないレジスタを変更することを表す
  let Defs = [FR];
  // NOTE "$src = $r1" $srcと$r1が同じレジスタとなる必要があるという制約
  let Constraints = "$src = $r1";
}
def ADDAREG : Comet2InstRegRegSet<0x24, "ADDA", add, IntRegs>;
def SUBAREG : Comet2InstRegRegSet<0x25, "SUBA", sub, IntRegs>;
def ANDREG  : Comet2InstRegRegSet<0x34, "AND",  and, IntRegs>;
def ORREG   : Comet2InstRegRegSet<0x35, "OR",   or,  IntRegs>;
def XORREG  : Comet2InstRegRegSet<0x36, "XOR",  xor, IntRegs>;


// 算術命令 register,address
class Comet2InstRegAdrSet<bits<8> op, string asmstr, SDNode OpNode>
    : Comet2InstRegAdr<op, (outs IntRegs:$r), (ins IntRegs:$src, mem:$adr),
                       !strconcat(asmstr, " $r,$adr"),
                       [(set IntRegs:$r, (OpNode i16:$src, (load addr:$adr)))]> {
  let Defs = [FR];
  let Constraints = "$src = $r";
}
def ADDA : Comet2InstRegAdrSet<0x20, "ADDA", add>;
def SUBA : Comet2InstRegAdrSet<0x21, "SUBA", sub>;
def AND  : Comet2InstRegAdrSet<0x30, "AND",  and>;
def OR   : Comet2InstRegAdrSet<0x31, "OR",   or>;
def XOR  : Comet2InstRegAdrSet<0x32, "XOR",  xor>;


// 算術命令 register,immediate,register
// (adr+x)をアドレスとしてではなく値として使う
class Comet2InstRegImmRegSet<bits<8> op, string asmstr, SDNode OpNode>
    : Comet2InstRegAdr<op, (outs IntRegs:$r), (ins IntRegs:$src, IntRegs:$x),
                       !strconcat(asmstr, " $r,0,$x"),
                       [(set IntRegs:$r, (OpNode IntRegs:$src, IntRegs:$x))]> {
  let Defs = [FR];
  let Constraints = "$src = $r";
}
def SLA : Comet2InstRegImmRegSet<0x50, "SLA",  shl>;
// TODO SLL 相当の命令
def SRA : Comet2InstRegImmRegSet<0x51, "SRA",  sra>;
def SRL : Comet2InstRegImmRegSet<0x53, "SRL",  srl>;


// Load/Store命令
// メモリロード
let Defs = [FR] in
def LD : Comet2InstRegAdr<0x10, (outs IntRegs:$r), (ins mem:$adr),
                          "LD $r,$adr", [(set IntRegs:$r, (load addr:$adr))]>;
// メモリストア
let mayStore = 1 in
def ST : Comet2InstRegAdr<0x11, (outs), (ins IntRegs:$src, IntRegs:$dest, i16imm:$offset),
                          "ST $src,$offset,$dest", []>;
// NOTE Pat 第1引数にマッチしたパターンを第2引数に変換する
// NOTE llvm/include/llvm/Target/TargetSelectionDAG.td Pat
// Comet2StRegOffsetはComet2ISelLowering.cpp内で記述された処理によって生成される
def : Pat<(Comet2StRegOffset IntRegs:$src, IntRegs:$dest, immSExt16:$offset),
          (ST IntRegs:$src, IntRegs:$dest, immSExt16:$offset)>;
// NOTE PatFragで複数階層のパターンを1階層として扱うことができる
// NOTE llvm/include/llvm/Target/TargetSelectionDAG.td PatFrag
// (以下の場合、PatFragなしでもPatは記述できる)
def StAddr : PatFrag<(ops node:$n1, node:$n2, node:$n3),
                     (store node:$n1, (addr node:$n2, node:$n3))>;
def : Pat<(StAddr IntRegs:$src, IntRegs:$dest, immSExt16:$offset),
          (ST IntRegs:$src, IntRegs:$dest, immSExt16:$offset)>;
// (store src, (add ...))よりも (store src, dest)のパターンが
// 優先してマッチするので下のPatは本来不要 (メモ用に残す)
def : Pat<(store IntRegs:$src, (add IntRegs:$dest, immSExt16:$offset)),
          (ST IntRegs:$src, IntRegs:$dest, immSExt16:$offset)>;
// 即値ロード
def LAD : Comet2InstRegAdr<0x12, (outs IntRegs:$r), (ins i16imm:$adr),
                           "LAD $r,$adr", [(set IntRegs:$r, immSExt16:$adr)]>;
// レジスタ間ロード
let Defs = [FR] in
def LDREG : Comet2InstRegReg<0x14, (outs IntRegs:$r1), (ins IntRegs:$r2),
                             "LD $r1,$r2", [(set IntRegs:$r1, IntRegs:$r2)]>;


// Call命令
let isCall = 1, r = 0, x = 0 in
def CALL : Comet2InstRegAdr<0x80, (outs), (ins calltarget:$adr, variable_ops),
                            "CALL $adr", []>;
def : Pat<(Comet2Call (i16 tglobaladdr:$dst)), (CALL tglobaladdr:$dst)>;
def : Pat<(Comet2Call (i16 texternalsym:$dst)), (CALL texternalsym:$dst)>;


// Ret命令
// NOTE Comet2CallingConv.tdでGR0に戻り値を格納する設定にしているため、Usesがなくてもimplicitが設定される
let isBranch = 1, isTerminator = 1, isBarrier = 1, isReturn = 1,
    x = 0, r = 0, adr = 0 in
def RET : Comet2InstRegAdr<0x81, (outs), (ins), "RET", [(Comet2Ret)]>;


// 比較命令
class CompReg<bits<8> op, string asmstr, SDNode OpNode>
    : Comet2InstRegReg<op, (outs), (ins IntRegs:$r1, IntRegs:$r2),
                       !strconcat(asmstr, " $r1,$r2"),
                       [(OpNode i16:$r1, i16:$r2)]> {
  let hasSideEffects = 1;
  let mayStore = 0;
  let mayLoad = 0;
  let Defs = [FR];
}
def CPAREG : CompReg<0x44, "CPA", Comet2Cpa>;
def CPLREG : CompReg<0x45, "CPL", Comet2Cpl>;


// 無条件分岐命令
let isBranch = 1, isTerminator = 1, isBarrier = 1, r = 0, x = 0 in
def JUMP : Comet2InstRegAdr<0x64, (outs), (ins brtarget:$target),
                            "JUMP $target", [(br bb:$target)]>;


// 分岐命令
class Branch<bits<8> op, string asmstr, SDNode OpNode>
    : Comet2InstRegAdr<op, (outs), (ins brtarget:$target),
                       !strconcat(asmstr, " $target"),
                       [(OpNode bb:$target)]> {
  let hasSideEffects = 0;
  let mayStore = 0;
  let mayLoad = 0;
  let isBranch = 1;
  let isTerminator = 1;
  // NOTE Uses オペランドに現れないレジスタを参照することを表す
  let Uses = [FR];
  let r = 0;
  let x = 0;
}
def JZE : Branch<0x63, "JZE", Comet2Jze>;  // ==
def JNZ : Branch<0x62, "JNZ", Comet2Jnz>;  // !=
def JPL : Branch<0x65, "JPL", Comet2Jpl>;  // >=
def JMI : Branch<0x61, "JMI", Comet2Jmi>;  // <=


// 分岐疑似命令
// Comet2にない比較命令を疑似命令にして、後で複数のComet2命令に変換する
// NOTE 疑似命令生成後の処理はexpandPostRAPseudo()
class BranchPseudo<SDNode OpNode>
    : Comet2InstPseudo<(outs), (ins brtarget:$target), "", [(OpNode bb:$target)]> {
  let hasSideEffects = 0;
  let mayStore = 0;
  let mayLoad = 0;
  let isBranch = 1;
  let isTerminator = 1;
  let Uses = [FR];
}
def JLEp : BranchPseudo<Comet2Jle>;  // >=
def JGEp : BranchPseudo<Comet2Jge>;  // <=


// Callの開始終了を表す疑似命令
let Defs = [GR7], Uses = [GR7] in {
def ADJCALLSTACKDOWN : Comet2InstPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                        "!ADJCALLSTACKDOWN $amt1",
                                        [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Comet2InstPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                        "!ADJCALLSTACKUP $amt1",
                                        [(callseq_end timm:$amt1, timm:$amt2)]>;
}
