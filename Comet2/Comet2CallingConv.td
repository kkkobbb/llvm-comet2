//===-- Comet2CallingConv.td - Calling Conventions COMET-II -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// レジスタの役割
// GR0     戻り値
// GR0-GR5 引数受け取り (GR0は戻り値格納と引数格納の両方で使用する)
// GR6-GR7 呼び出し先退避

// 引数
def CC_Comet2 : CallingConv<[
    // i8型の引数はi16型に昇格する
    CCIfType<[i8], CCPromoteToType<i16>>,
    // i16型はGR1-GR4に渡す
    CCIfType<[i16], CCAssignToReg<[GR0, GR1, GR2, GR3, GR4, GR5]>>,
    // スタックを使用するように設定するが、レジスタに収まらなかった場合、エラー
    //   この設定を消すとComet2側に処理がいかず、UNREACHABLEのエラーになる
    // NOTE llvm/include/llvm/Target/TargetCallingConv.td CCAssignToStack
    // NOTE CCAssignToStack<確保するサイズ、アラインメント> (0の場合ABIの値を使用する)
    CCIfType<[i16], CCAssignToStack<0, 0>>
]>;

// 戻り値
def RetCC_Comet2 : CallingConv<[
    // i16型はGR0に渡す
    CCIfType<[i16], CCAssignToReg<[GR0]>>
]>;

// 呼び出し先退避レジスタ
def CSR : CalleeSavedRegs<(add GR6, GR7)>;
