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

include "Comet2InstrFormats.td"

// 即値ロード用のオペランド. 16bit
// 16-0: 符号付き16bit整数
// EncoderMethod: bit列から符号付き16bit整数を取得
def imm16 : Operand<i16> {
  let EncoderMethod = "get16bitOpValue";
}

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

// 比較命令(CPA,CPL)と分岐命令(JZE, ...)を繋ぐための仮想的なオペランド
// TODO implicit defs uses とかを使う？
def condtarget : Operand<i16>;

// 符号付き16bit整数
// 定数(ISD::Constant)で16bitで表現可能なもの
def immSExt16 : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

// NOTE llvm/include/llvm/CodeGen/SDNodeProperties.td
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], [SDNPWantRoot]>;


// 算術命令 register,register
// 本来不要だが、このクラスだけ練習用としてRegisterClassを引数に持つようにしている
// NOTE 入力と出力が同じレジスタの場合の命令は AVR ADD命令が参考になるかも
class Comet2InstRegRegSet<bits<8> op, string asmstr, SDNode OpNode, RegisterClass RC>
    : Comet2InstRegReg<op, (outs RC:$r1), (ins RC:$src, RC:$r2),
                       !strconcat(asmstr, " $r1,$r2"),
                       [(set i16:$r1, (OpNode i16:$src, i16:$r2))]> {
  let Constraints = "$src = $r1";
}
def ADDAREG : Comet2InstRegRegSet<0x20, "ADDA", add, IntRegs>;
def SUBAREG : Comet2InstRegRegSet<0x21, "SUBA", sub, IntRegs>;
def XORREG  : Comet2InstRegRegSet<0x36, "XOR",  xor, IntRegs>;


// 算術命令 register,address
class Comet2InstAdrSet<bits<8> op, string asmstr, SDNode OpNode>
    : Comet2InstRegAdr<op, (outs IntRegs:$r), (ins IntRegs:$src, imm16:$adr),
                       !strconcat(asmstr, " $r,$adr"),
                       [(set IntRegs:$r, (OpNode i16:$src, immSExt16:$adr))]> {
  let Constraints = "$src = $r";
}
def ADDL : Comet2InstAdrSet<0x22, "ADDL", add>;
def XOR  : Comet2InstAdrSet<0x32, "XOR",  xor>;


// Load/Store命令
// メモリロード
def LD : Comet2InstRegAdr<0x10, (outs IntRegs:$r), (ins mem:$adr),
                          "LD $r,$adr", [(set IntRegs:$r, (load addr:$adr))]>;
// メモリストア
def ST : Comet2InstRegAdr<0x11, (outs), (ins IntRegs:$r, mem:$adr),
                          "ST $r,$adr", [(store IntRegs:$r, addr:$adr)]>;
// 即値ロード
def LAD : Comet2InstRegAdr<0x12, (outs IntRegs:$r), (ins imm16:$adr),
                           "LAD $r,$adr", [(set IntRegs:$r, immSExt16:$adr)]>;
// レジスタ間ロード
def LDREG : Comet2InstRegReg<0x14, (outs IntRegs:$r1), (ins IntRegs:$r2),
                             "LD $r1,$r2", [(set IntRegs:$r1, IntRegs:$r2)]>;


// Call命令
def SDT_Comet2Call : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def Comet2Call : SDNode<"Comet2ISD::Call", SDT_Comet2Call,
                        [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                         SDNPVariadic]>;

let isCall = 1, r = 0, x = 0 in
def CALL : Comet2InstRegAdr<0x80, (outs), (ins calltarget:$adr, variable_ops),
                            "CALL $adr", []>;

// Pat 第1引数にマッチしたパターンを第2引数に変換する
// NOTE llvm/include/llvm/Target/TargetSelectionDAG.td
def : Pat<(Comet2Call (i16 tglobaladdr:$dst)), (CALL tglobaladdr:$dst)>;
def : Pat<(Comet2Call (i16 texternalsym:$dst)), (CALL texternalsym:$dst)>;


// Ret命令
def Comet2Ret : SDNode<"Comet2ISD::Ret", SDTNone,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

let isBranch = 1, isTerminator = 1, isBarrier = 1, isReturn = 1, Uses = [GR0],
    x = 0, r = 0, adr = 0 in
def RET : Comet2InstRegAdr<0x81, (outs), (ins), "RET", [(Comet2Ret)]>;


// 比較命令
class CompReg<bits<8> op, string asmstr>
    : Comet2InstRegReg<op, (outs condtarget:$cond), (ins IntRegs:$r1, IntRegs:$r2),
                       !strconcat(asmstr, " $r1,$r2"), []> {
  let hasSideEffects = 1;  // TODO これでいいかは不明
  let mayStore = 0;
  let mayLoad = 0;
}
def CPAREG : CompReg<0x44, "CPA">;
def CPLREG : CompReg<0x45, "CPL">;


// 無条件分岐命令
let isBranch = 1, isTerminator = 1, isBarrier = 1, r = 0, x = 0 in
def JUMP : Comet2InstRegAdr<0x64, (outs), (ins brtarget:$target),
                            "JUMP $target", [(br bb:$target)]>;


// 分岐命令
class Branch<bits<8> op, string asmstr>
    : Comet2InstRegAdr<op, (outs), (ins condtarget:$cond, brtarget:$target),
                       !strconcat(asmstr, " $target"), []> {
  let hasSideEffects = 0;  // TODO これでいいかは不明
  let mayStore = 0;
  let mayLoad = 0;
  let isBranch = 1;
  let isTerminator = 1;
  let r = 0;
  let x = 0;
}
def JZE : Branch<0x63, "JZE">;
def JNZ : Branch<0x62, "JNZ">;
def JPL : Branch<0x65, "JPL">;
def JMI : Branch<0x61, "JMI">;

// lhs == rhs
def : Pat<(brcond (i16 (seteq  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JZE (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setueq IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JZE (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
// lhs != rhs
def : Pat<(brcond (i16 (setne  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JNZ (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setune IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JNZ (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
// lhs > rhs
def : Pat<(brcond (i16 (setlt  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JPL (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setult IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JPL (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
// lhs < rhs
def : Pat<(brcond (i16 (setgt  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JMI (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setugt IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JMI (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;


// 分岐疑似命令
// Comet2にない比較命令を疑似命令にして、後で複数のComet2命令に変換する
// NOTE 疑似命令生成後の処理はexpandPostRAPseudo()
class BranchPseudo : Comet2InstPseudo<(outs), (ins condtarget:$cond, brtarget:$target),
                                      "", []> {
  let hasSideEffects = 0;  // TODO これでいいかは不明
  let mayStore = 0;
  let mayLoad = 0;
  let isBranch = 1;
  let isTerminator = 1;
}
def JLEp : BranchPseudo;  // >=
def JGEp : BranchPseudo;  // <=

// lhs >= rhs
def : Pat<(brcond (i16 (setle  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JLEp (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setule IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JLEp (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
// lhs <= rhs
def : Pat<(brcond (i16 (setge  IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JGEp (CPAREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;
def : Pat<(brcond (i16 (setuge IntRegs:$lhs, IntRegs:$rhs)), bb:$dst),
          (JGEp (CPLREG IntRegs:$lhs, IntRegs:$rhs), bb:$dst)>;


// その他疑似命令

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

let Defs = [GR7], Uses = [GR7] in {
def ADJCALLSTACKDOWN : Comet2InstPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                        "!ADJCALLSTACKDOWN $amt1",
                                        [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Comet2InstPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                        "!ADJCALLSTACKUP $amt1",
                                        [(callseq_end timm:$amt1, timm:$amt2)]>;
}
