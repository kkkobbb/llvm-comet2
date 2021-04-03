; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @main() {
; CHECK-LABEL: main NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,16
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,14,GR7
; CHECK-NEXT:    ST GR2,12,GR7
; CHECK-NEXT:    ST GR3,10,GR7
; CHECK-NEXT:    ST GR4,8,GR7
; CHECK-NEXT:    LAD GR6,8
; CHECK-NEXT:    ST GR6,6,GR7
; CHECK-NEXT:    LAD GR6,7
; CHECK-NEXT:    ST GR6,4,GR7
; CHECK-NEXT:    LAD GR6,6
; CHECK-NEXT:    ST GR6,2,GR7
; CHECK-NEXT:    LAD GR6,5
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:    LAD GR1,1
; CHECK-NEXT:    LAD GR2,2
; CHECK-NEXT:    LAD GR3,3
; CHECK-NEXT:    LAD GR4,4
; CHECK-NEXT:    CALL test
; CHECK-NEXT:    LD GR4,8,GR7
; CHECK-NEXT:    LD GR3,10,GR7
; CHECK-NEXT:    LD GR2,12,GR7
; CHECK-NEXT:    LD GR1,14,GR7
; CHECK-NEXT:    LAD GR6,16
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
entry:
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d, i16 %e, i16 %f, i16 %g, i16 %h) {
; CHECK-LABEL: test NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,6
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,4,GR7
; CHECK-NEXT:    ST GR2,2,GR7
; CHECK-NEXT:    ST GR3,0,GR7
; CHECK-NEXT:    ADDA GR3,GR4
; CHECK-NEXT:    ADDA GR1,GR2
; CHECK-NEXT:    ADDA GR1,GR3
; CHECK-NEXT:    LD GR6,10,GR7
; CHECK-NEXT:    ADDA GR6,12,GR7
; CHECK-NEXT:    LD GR2,6,GR7
; CHECK-NEXT:    ADDA GR2,8,GR7
; CHECK-NEXT:    ADDA GR2,GR6
; CHECK-NEXT:    ADDA GR1,GR2
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR3,0,GR7
; CHECK-NEXT:    LD GR2,2,GR7
; CHECK-NEXT:    LD GR1,4,GR7
; CHECK-NEXT:    LAD GR6,6
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %ret1 = add i16 %a, %b
  %ret2 = add i16 %c, %d
  %ret3 = add i16 %e, %f
  %ret4 = add i16 %g, %h
  %ret5 = add i16 %ret1, %ret2
  %ret6 = add i16 %ret3, %ret4
  %ret7 = add i16 %ret5, %ret6
  ret i16 %ret7
}

