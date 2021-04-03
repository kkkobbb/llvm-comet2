; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @br_uncond() {
; CHECK-LABEL: br_uncond NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    LAD GR6,0
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:  ; %bb.1:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:  ; %bb.2:
; CHECK-NEXT:    LD GR6,0,GR7
; CHECK-NEXT:    LD GR0,GR6
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
entry:
  %b = alloca i16
  store i16 0, i16* %b
  br label %lbl2

lbl1:
  store i16 1, i16* %b
  br label %lbl3

lbl2:
  store i16 2, i16* %b
  br label %lbl3

lbl3:
  %result = load i16, i16* %b
  ret i16 %result
}
