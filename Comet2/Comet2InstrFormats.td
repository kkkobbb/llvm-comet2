//===-- Comet2InstrFormats.td - COMET II Instruction Formats -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class Format<bits<2> val> {
  bits<2> Value = val;
}

def InstFormatPseudo : Format<0>;
def InstFormatAdr    : Format<1>;
def InstFormatReg    : Format<2>;

class Comet2Inst2words<dag outs, dag ins, string asmstr,
    list<dag> pattern, InstrItinClass itin, Format f>
    : Instruction {
    field bits<32> Inst;
    Format Form = f;

    bits<8> Opcode = 0;

    let Namespace = "Comet2";
    let Size = 4;  // TODO 語長? バイトサイズ?
    let Inst{31-24} = Opcode;
    let OutOperandList = outs;
    let InOperandList = ins;
    let AsmString = asmstr;
    let Pattern = pattern;
    let Itinerary = itin;

    bits<2> FormBits = Form.Value;

    let DecoderNamespace = "Comet2";

    field bits<32> SoftFail = 0;
}

class Comet2InstFormAdr<bits<8> op, dag outs, dag ins, string asmstr,
    list<dag> pattern, InstrItinClass itin>
    : Comet2Inst2words<outs, ins, asmstr, pattern, itin, InstFormatAdr> {
    bits<4> r;
    bits<4> x;
    bits<16> adr;

    let Opcode = op;

    let Inst{23-20} = r;
    let Inst{19-16} = x;
    let Inst{15-0} = adr;
}

class Comet2InstFormReg<bits<8> op, dag outs, dag ins, string asmstr,
    list<dag> pattern, InstrItinClass itin>
    : Instruction {
    field bits<16> Inst;

    bits<8> Opcode = op;
    bits<4> r1;
    bits<4> r2;
    let Inst{15-8} = Opcode;
    let Inst{7-4} = r1;
    let Inst{3-0} = r2;

    let Namespace = "Comet2";
    let Size = 1;  // TODO 語長? バイトサイズ?
    let OutOperandList = outs;
    let InOperandList = ins;
    let AsmString = asmstr;
    let Pattern = pattern;
    let Itinerary = itin;

    Format Form = InstFormatReg;
    bits<2> FormBits = Form.Value;

    let DecoderNamespace = "Comet2";

    field bits<32> SoftFail = 0;
}

class Comet2Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Comet2Inst2words<outs, ins, asmstr, pattern, IICPseudo, InstFormatPseudo> {
    let isCodeGenOnly = 1;
    let isPseudo = 1;
}
