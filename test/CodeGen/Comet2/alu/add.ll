; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @add_reg_const(i16 %var1) {
; CHECK-LABEL: add_reg_const NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR1,2
; CHECK-NEXT:    ADDA GR0,GR1
; CHECK-NEXT:    RET
  %result = add i16 %var1, 2
  ret i16 %result
}

define i16 @add_reg_ptr(i16 %var1, i16* %var2_ptr) {
; CHECK-LABEL: add_reg_ptr NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    ADDA GR0,0,GR1
; CHECK-NEXT:    RET
  %var2 = load i16, i16* %var2_ptr
  %result = add i16 %var1, %var2
  ret i16 %result
}

define i16 @add_reg_reg(i16 %var1, i16 %var2) {
; CHECK-LABEL: add_reg_reg NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    ADDA GR0,GR1
; CHECK-NEXT:    RET
  %result = add i16 %var1, %var2
  ret i16 %result
}
