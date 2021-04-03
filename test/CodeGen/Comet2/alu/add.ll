; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @add_alloc_alloc() {
; CHECK-LABEL: add_alloc_alloc NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:    LAD GR6,1
; CHECK-NEXT:    ST GR6,2,GR7
; CHECK-NEXT:    LAD GR6,3
; CHECK-NEXT:    LD GR0,GR6
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %a = alloca i16
  store i16 1, i16* %a
  %b = alloca i16
  store i16 2, i16* %b
  %var1 = load i16, i16* %a
  %var2 = load i16, i16* %b
  %result = add i16 %var1, %var2
  ret i16 %result
}

define i16 @add_reg_alloc(i16 %var1) {
; CHECK-LABEL: add_reg_alloc NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,2,GR7
; CHECK-NEXT:    LAD GR6,1
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:    ADDA GR1,GR6
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,2,GR7
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %a = alloca i16
  store i16 1, i16* %a
  %var2 = load i16, i16* %a
  %result = add i16 %var1, %var2
  ret i16 %result
}

define i16 @add_reg_const(i16 %var1) {
; CHECK-LABEL: add_reg_const NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR1,GR6
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %result = add i16 %var1, 2
  ret i16 %result
}

define i16 @add_reg_ptr(i16 %var1, i16* %var2_ptr) {
; CHECK-LABEL: add_reg_ptr NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    ADDA GR1,0,GR2
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %var2 = load i16, i16* %var2_ptr
  %result = add i16 %var1, %var2
  ret i16 %result
}

define i16 @add_reg_reg(i16 %var1, i16 %var2) {
; CHECK-LABEL: add_reg_reg NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    ADDA GR1,GR2
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %result = add i16 %var1, %var2
  ret i16 %result
}
