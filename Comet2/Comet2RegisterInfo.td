//===-- Comet2RegisterInfo.td - COMET-II Register defs ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the COMET-II register files
//===----------------------------------------------------------------------===//

class Comet2Reg<bits<4> Enc, string n> : Register<n> {
  let HWEncoding{3-0} = Enc;
  let Namespace = "Comet2";
}

def GR0 : Comet2Reg<0, "GR0">, DwarfRegNum<[0]>;
def GR1 : Comet2Reg<1, "GR1">, DwarfRegNum<[1]>;
def GR2 : Comet2Reg<2, "GR2">, DwarfRegNum<[2]>;
def GR3 : Comet2Reg<3, "GR3">, DwarfRegNum<[3]>;
def GR4 : Comet2Reg<4, "GR4">, DwarfRegNum<[4]>;
def GR5 : Comet2Reg<5, "GR5">, DwarfRegNum<[5]>;
def GR6 : Comet2Reg<6, "GR6">, DwarfRegNum<[6]>;
def GR7 : Comet2Reg<7, "GR7">, DwarfRegNum<[7]>;

def IntRegsAll : RegisterClass<"Comet2", [i16], 16,
    (add GR0, GR1, GR2, GR3, GR4, GR5, GR6, GR7)>;

def IntRegsArgs : RegisterClass<"Comet2", [i16], 16,
    (add GR5, GR6)>;

def IntRegs : RegisterClass<"Comet2", [i16], 16,
    (add GR1, GR2, GR3, GR4, GR5, GR6)>;
