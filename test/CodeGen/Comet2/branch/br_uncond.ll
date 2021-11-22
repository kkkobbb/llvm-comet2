; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @br_uncond() noinline optnone {
; CHECK-LABEL: br_uncond NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    JUMP LBB0_1
; CHECK-NEXT:  LBB0_1 NOP                              ; %lbl2
; CHECK-NEXT:    LAD GR0,1
; CHECK-NEXT:    RET
entry:
  br label %lbl2

lbl1:
  ret i16 0

lbl2:
  ret i16 1

lbl3:
  ret i16 2
}
