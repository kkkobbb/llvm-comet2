; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @main() {
; CHECK-LABEL: main NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR0,1
; CHECK-NEXT:    CALL callee
; CHECK-NEXT:    RET
entry:
  %0 = call i16 @callee(i16 1)
  ret i16 %0
}

define i16 @callee(i16) {
; CHECK-LABEL: callee NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    RET
entry:
  ret i16 %0
}
