//===-- Comet2InstrInfo.td - Target Description for COMET II -*- tablegen -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file describes the COMET-II instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "Comet2InstrFormats.td"

class Comet2InstReg<bits<8> op, string asmstr, SDNode OpNode, InstrItinClass itin,
    RegisterClass RC>
    : Comet2InstFormReg<op, (outs RC:$r1), (ins RC:$r1, RC:$r2),  // TODO in,outが同じレジスタ これでいい?
                        !strconcat(asmstr, " $r1, $r2"),
                        [(set RC:$r1, (OpNode RC:$r1, RC:$r2))], itin> {
}

def ADDA : Comet2InstReg<0x20, "ADDA", add, IIAlu, IntRegs>;
def SUBA : Comet2InstReg<0x21, "SUBA", sub, IIAlu, IntRegs>;


// 即値ロード
class LoadI<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs IntRegs:$r), (ins i16imm:$addr),
                        !strconcat(asmstr, " $r, $addr"),
                        [(set IntRegs:$r, i16imm:$addr)], IICLoad> {
}

def LAD : LoadI<0x12, "LAD">;


// Call命令
def SDT_Comet2Call : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

def Comet2Call : SDNode<"Comet2ISD::Call", SDT_Comet2Call,
                        [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                         SDNPVariadic]>;

// Pat 第1引数にマッチしたパターンを第2引数に変換する
def Pat<Comet2Call (i16, tglobaladdr:$dst)),
        (CALL tglobaladdr:$dst)>;
def Pat<Comet2Call (i16 texternalsym:$dst),
        (CALL texternalsym)>;

class Call<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs), (ins i16imm:$addr, variable_ops),
                        !strconcat(asmstr, " $addr"), [(Comet2Call imm:$addr)],
                        IICBranch> {
    let isCall = 1;
    let DecoderMethod = "DecodeCallTarget";
}

def CALL : Call<0x80, "CALL">;


// Ret命令
def SDT_Comet2Ret : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def Comet2Ret : SDNode<"Comet2ISD::Ret", SDT_Comet2Ret,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

class RetInst<bits<8> op, string asmstr>
    : Comet2InstFormAdr<op, (outs), (ins), asmstr,
                        [(Comet2Ret)], IICBranch> {
    let isBranch = 1;
    let isTerminator = 1;
    let isBarrier = 1;
    let isReturn = 1;
}


def mem : Operand<i16> {
    let PrintMethod = "printMemOperand";
    let MIOperandInfo = (ops IntRegs, i16imm);
    let EncoderMethod = "getMemEncoding";
}

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

// Load/Store命令共通フォーマット
class FMem<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern,
    InstrItinClass itin>
    : Comet2InstFormAdr<op, outs, ins, asmstr, pattern, itin> {
    bits<16> addr;
    let Inst{15-0} = addr;
    let DecoderMethod = "DecodeMem";
}

// Load命令
let canFoldAsLoad = 1 in
class LoadM<bits<8> op, string asmstr, RegisterClass RC>
    : FMem<op, (outs RC:$r), (ins mem:$addr),
           !strconcat(asmstr, " $r $addr"),
           [(set RC:$r, (load addr:$addr))], IICLoad>;

// Store命令
class StoreM<bits<8> op, string asmstr, RegisterClass RC>
    : FMem<op, (outs), (ins RC:$r, mem:$addr),
           !strconcat(asmstr, " $r $addr),
           [(store RC:$r, addr:$addr)], IICStore>;

def LD : LoadM<0x10, "LD", IntRegs>;
def ST : StoreM<0x11, "ST", IntRegs>;

