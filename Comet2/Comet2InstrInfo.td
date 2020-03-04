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

// NOTE AVR ADD命令が参考になるかも
// Add命令
class Comet2InstReg<bits<8> op, string asmstr, SDNode OpNode, RegisterClass RC>
    : Comet2InstFormReg<op, (outs RC:$r1), (ins RC:$r2, RC:$src),
                        !strconcat(asmstr, " $r1,$r2"),
                        [(set i16:$r1, (OpNode i16:$src, i16:$r2))]> {
  let Constraints = "$src = $r1";
}

def ADDAREG : Comet2InstReg<0x20, "ADDA", add, IntRegs>;
def SUBAREG : Comet2InstReg<0x21, "SUBA", sub, IntRegs>;


// 即値ロード用のオペランド. 16bit
// 16-0: 符号付き16bit整数
// EncoderMethod: bit列から符号付き16bit整数を取得
def imm16 : Operand<i16> {
  let EncoderMethod = "get16bitOpValue";
}

def calltarget  : Operand<iPTR> {
  let EncoderMethod = "getCallTargetOpValue";
}

// 符号付き16bit整数
// 定数(ISD::Constant)で16bitで表現可能なもの
def immSExt16 : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

// 即値ロード
class LoadI<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs IntRegs:$r), (ins imm16:$addr),
                        !strconcat(asmstr, " $r,$addr"),
                        [(set IntRegs:$r, immSExt16:$addr)]> {
}

def LAD : LoadI<0x12, "LAD">;


// レジスタ間ロード
class LoadR<bits<8> op, string asmstr>
    : Comet2InstFormReg<op, (outs IntRegs:$r1), (ins IntRegs:$r2),
                        !strconcat(asmstr, " $r1,$r2"),
                        [(set IntRegs:$r1, IntRegs:$r2)]> {
}

def LDREG : LoadR<0x14, "LD">;


// Call命令
def SDT_Comet2Call : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def Comet2Call : SDNode<"Comet2ISD::Call", SDT_Comet2Call,
                        [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                         SDNPVariadic]>;

class Call<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs), (ins calltarget:$adr, variable_ops),
                        !strconcat(asmstr, " $adr"), [(Comet2Call imm:$adr)]> {
    let isCall = 1;
    let DecoderMethod = "DecodeCallTarget";
    let r = 0;
    let x = 0;
}

def CALL : Call<0x80, "CALL">;

// Pat 第1引数にマッチしたパターンを第2引数に変換する
def : Pat<(Comet2Call (i16 tglobaladdr:$dst)),
          (CALL tglobaladdr:$dst)>;
def : Pat<(Comet2Call (i16 texternalsym:$dst)),
          (CALL texternalsym:$dst)>;


// Ret命令
def SDT_Comet2Ret : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def Comet2Ret : SDNode<"Comet2ISD::Ret", SDT_Comet2Ret,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

class RetInst<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs), (ins IntRegs:$r), asmstr,
                        [(Comet2Ret IntRegs:$r)]> {
    let isBranch = 1;
    let isTerminator = 1;
    let isBarrier = 1;
    let isReturn = 1;
    let x = 0;
    let adr = 0;
}

def RET : RetInst<0x81, "RET">;


def mem : Operand<i16> {
    let PrintMethod = "printMemOperand";
    let MIOperandInfo = (ops IntRegs, i16imm);
    let EncoderMethod = "getMemEncoding";
}

// NOTE llvm/include/llvm/CodeGen/SDNodeProperties.td
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], [SDNPWantRoot]>;

// Load/Store命令共通フォーマット
class FMem<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern>
    : Comet2InstFormAdr<op, outs, ins, asmstr, pattern> {
    bits<16> addr;
    let Inst{15-0} = addr;
    let DecoderMethod = "DecodeMem";
}

// Load命令
class LoadM<bits<8> op, string asmstr, RegisterClass RC>
    : FMem<op, (outs RC:$r), (ins mem:$addr),
           !strconcat(asmstr, " $r,$addr"),
           [(set RC:$r, (load addr:$addr))]>;

// Store命令
class StoreM<bits<8> op, string asmstr, RegisterClass RC>
    : FMem<op, (outs), (ins RC:$r, mem:$addr),
           !strconcat(asmstr, " $r,$addr"),
           [(store RC:$r, addr:$addr)]>;

def LD : LoadM<0x10, "LD", IntRegs>;
def ST : StoreM<0x11, "ST", IntRegs>;


// TODO brcond br の実装


// 疑似命令

// Target-independent type requirements, but with target-specific formats.
def SDT_CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>,
                                       SDTCisVT<1, i16>]>;
def SDT_CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>,
                                     SDTCisVT<1, i16>]>;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

let Defs = [GR7], Uses = [GR7] in {
def ADJCALLSTACKDOWN : Comet2Pseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                  "!ADJCALLSTACKDOWN $amt1",
                                  [(callseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Comet2Pseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                                  "!ADJCALLSTACKUP $amt1",
                                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}
