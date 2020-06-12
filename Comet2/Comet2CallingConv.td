//===-- Comet2CallingConv.td - Calling Conventions COMET-II -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


// TODO CallingConvは不要？

// レジスタの役割
// GR0     戻り値
// GR1-GR4 引数受け取り
// GR5     空き
// GR6     フレームレジスタ退避等の計算用
// GR7     フレームレジスタ

// 引数
def CC_Comet2 : CallingConv<[
    // i8型の引数はi16型に昇格する
    CCIfType<[i8], CCPromoteToType<i16>>,
    // i16型はGR1-GR4に渡す
    CCIfType<[i16], CCAssignToReg<[GR1, GR2, GR3, GR4]>>,
]>;

// 戻り値
def RetCC_Comet2 : CallingConv<[
    // i16型はGR0に渡す
    CCIfType<[i16], CCAssignToReg<[GR0]>>
]>;

// 呼び出し先退避レジスタ
// GR0は戻り値格納用、GR6は計算用、GR7はフレームレジスタとして使用する
def CSR : CalleeSavedRegs<(add (sequence "GR%u", 1, 5))>;

