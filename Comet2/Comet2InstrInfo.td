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


// TODO 即値の表現 LAD命令


// TODO Call命令


// TODO Ret命令


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

