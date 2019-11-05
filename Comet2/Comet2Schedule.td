//===-- Comet2InstrInfo.td - Target Description for COMET-II -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// TODO スケジュールは不要？
// NOTE Sparc参考

// 機能ユニット
def ALU : FuncUnit;

// 命令スケジュール (Instruction Itinerary)
def IICAlu    : InstrItinClass;
def IICLoad   : InstrItinClass;
def IICStore  : InstrItinClass;
def IICBranch : InstrItinClass;
def IICPseudo : InstrItinClass;

def Comet2GenericItineraries : ProcessorItineraries<[ALU], [], [
    InstrItinData<IICAlu,    [InstrStage<1, [ALU]>]>,
    InstrItinData<IICLoad,   [InstrStage<1, [ALU]>]>,
    InstrItinData<IICStore,  [InstrStage<1, [ALU]>]>,
    InstrItinData<IICBranch, [InstrStage<1, [ALU]>]>,
    InstrItinData<IICPseudo, [InstrStage<1, [ALU]>]>
]>;

