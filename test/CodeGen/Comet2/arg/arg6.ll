; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @main() {
; CHECK-LABEL: main NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR0,1
; CHECK-NEXT:    LAD GR1,2
; CHECK-NEXT:    LAD GR2,3
; CHECK-NEXT:    LAD GR3,4
; CHECK-NEXT:    LAD GR4,5
; CHECK-NEXT:    LAD GR5,6
; CHECK-NEXT:    CALL test
; CHECK-NEXT:    RET
entry:
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4, i16 5, i16 6)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d, i16 %e, i16 %f) {
; CHECK-LABEL: test NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    ADDA GR2,GR3
; CHECK-NEXT:    ADDA GR0,GR1
; CHECK-NEXT:    ADDA GR0,GR2
; CHECK-NEXT:    ADDA GR4,GR5
; CHECK-NEXT:    ADDA GR4,GR0
; CHECK-NEXT:    LD GR0,GR4
; CHECK-NEXT:    RET
  %ret1 = add i16 %a, %b
  %ret2 = add i16 %c, %d
  %ret3 = add i16 %e, %f
  %ret4 = add i16 %ret1, %ret2
  %ret5 = add i16 %ret3, %ret4
  ret i16 %ret5
}
