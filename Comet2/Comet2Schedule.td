//===-- Comet2InstrInfo.td - Target Description for COMET-II -*- tablegen -*-===//
//
//===----------------------------------------------------------------------===//

// TODO スケジュールは不要？

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

