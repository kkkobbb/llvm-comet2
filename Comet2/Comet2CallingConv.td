//===-- Comet2CallingConv.td - Calling Conventions COMET-II -*- tablegen -*-===//
//
//===----------------------------------------------------------------------===//


// TODO CallingConvは不要？

// 引数
def CC_Comet2 : CallingConv<[
    // i8型の引数はi16型に昇格する
    CCIfType<[i8], CCPromoteToType<i32>>,
    // i16型はGR1-GR7に渡す
    CCIfType<[i16], CCAssingToReg<[(sequence "GR%u", 1, 7)]>>,
]>;

// 戻り値
def RetCC_Comet2 : CallingConv<[
    // i16型はGR0に渡す
    CCIfType<[i16], CCAssingToReg<[GR0]>>
]>;

// 呼び出し先退避レジスタ
def CSR_Comet2 : CalleeSavedRegs<(add (sequence "GR%u", 1, 7))>;

