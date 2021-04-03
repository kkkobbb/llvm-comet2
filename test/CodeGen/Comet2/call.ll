; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @main() {
; CHECK-LABEL: main NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,16
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,14,GR7
; CHECK-NEXT:    LAD GR1,1
; CHECK-NEXT:    CALL callee
; CHECK-NEXT:    LD GR1,14,GR7
; CHECK-NEXT:    LAD GR6,16
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
entry:
  %0 = call i16 @callee(i16 1)
  ret i16 %0
}

define i16 @callee(i16) {
; CHECK-LABEL: callee NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    ST GR1,0,GR7
; CHECK-NEXT:    LD GR0,GR1
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
entry:
  %a = alloca i16
  store i16 %0, i16* %a
  %var = load i16, i16* %a
  ret i16 %var
}
