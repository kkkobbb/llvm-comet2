//===-- Comet2InstrFormats.td - COMET II Instruction Formats -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// NOTE llvm/include/llvm/Target/Target.td

class Comet2Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  let Namespace = "Comet2";

  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;

  field bits<32> SoftFail = 0;
}

class Comet2InstRegAdr<bits<8> opcode, dag outs, dag ins, string asmstr,
                        list<dag> pattern>
    : Comet2Inst<outs, ins, asmstr, pattern> {
  field bits<32> Inst;
  let Size = 4;  // NOTE バイトサイズ?

  bits<4> r;
  bits<4> x;
  bits<16> adr;

  let Inst{31-24} = opcode;
  let Inst{23-20} = r;
  let Inst{19-16} = x;
  let Inst{15-0} = adr;
}

class Comet2InstRegReg<bits<8> opcode, dag outs, dag ins, string asmstr,
                        list<dag> pattern>
    : Comet2Inst<outs, ins, asmstr, pattern> {
  field bits<16> Inst;
  let Size = 2;  // NOTE バイトサイズ?

  bits<4> r1;
  bits<4> r2;

  let Inst{15-8} = opcode;
  let Inst{7-4} = r1;
  let Inst{3-0} = r2;
}

class Comet2InstPseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Comet2Inst<outs, ins, asmstr, pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}
