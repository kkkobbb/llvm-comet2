; RUN: not --crash llc -mtriple=comet2 -verify-machineinstrs < %s 2>&1 | FileCheck %s

; CHECK: LLVM ERROR: Comet2 cannot read or overwrite SP

define i16 @add_alloc_alloc() {
  %a = alloca i16
  store i16 1, i16* %a
  %b = alloca i16
  store i16 2, i16* %b
  %var1 = load i16, i16* %a
  %var2 = load i16, i16* %b
  %result = add i16 %var1, %var2
  ret i16 %result
}
