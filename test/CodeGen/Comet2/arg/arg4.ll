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
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d) {
; CHECK-LABEL: test NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,2,GR7
; CHECK-NEXT:    ST GR3,0,GR7
; CHECK-NEXT:    ADDA GR3,GR4
; CHECK-NEXT:    ADDA GR1,GR2
; CHECK-NEXT:    ADDA GR1,GR3
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LD GR3,0,GR7
; CHECK-NEXT:    LD GR1,2,GR7
; CHECK-NEXT:    LAD GR6,4
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
  %ret1 = add i16 %a, %b
  %ret2 = add i16 %c, %d
  %ret5 = add i16 %ret1, %ret2
  ret i16 %ret5
}
