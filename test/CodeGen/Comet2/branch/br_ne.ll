; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @br_ne(i16 %a, i16 %b) {
; CHECK-LABEL: br_ne NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    CPA GR0,GR1
; CHECK-NEXT:    JZE LBB0_2
; CHECK-NEXT:    JUMP LBB0_1
; CHECK-NEXT:  LBB0_1 NOP                              ; %then
; CHECK-NEXT:    LAD GR0,0
; CHECK-NEXT:    RET
; CHECK-NEXT:  LBB0_2 NOP                              ; %else
; CHECK-NEXT:    LAD GR0,1
; CHECK-NEXT:    RET
entry:
  %0 = icmp ne i16 %a, %b
  br i1 %0, label %then, label %else

then:
  ret i16 0

else:
  ret i16 1
}
