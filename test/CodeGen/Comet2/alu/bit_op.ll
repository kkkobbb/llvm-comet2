; RUN: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @bit_op_and(i16 %a, i16 %b) {
; CHECK-LABEL: bit_op_and NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    AND GR0,GR1
; CHECK-NEXT:    RET
  %result = and i16 %a, %b
  ret i16 %result
}

define i16 @bit_op_or(i16 %a, i16 %b) {
; CHECK-LABEL: bit_op_or NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    OR GR0,GR1
; CHECK-NEXT:    RET
  %result = or i16 %a, %b
  ret i16 %result
}

define i16 @bit_op_xor(i16 %a, i16 %b) {
; CHECK-LABEL: bit_op_xor NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    XOR GR0,GR1
; CHECK-NEXT:    RET
  %result = xor i16 %a, %b
  ret i16 %result
}
