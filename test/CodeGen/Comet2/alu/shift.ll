; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @shift_left_arith(i16 %var1, i16 %var2) {
; CHECK-LABEL: shift_left_arith NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    SLA GR1,0,GR2
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %result = shl i16 %var1, %var2
  ret i16 %result
}

define i16 @shift_right_arith(i16 %var1, i16 %var2) {
; CHECK-LABEL: shift_right_arith NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    SRA GR1,0,GR2
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR1,0,GR7
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %result = ashr i16 %var1, %var2
  ret i16 %result
}
