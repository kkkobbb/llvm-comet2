; RUN: llc -mtriple=comet2 < %s | FileCheck %s
; RUN_: llc -mtriple=comet2 -verify-machineinstrs < %s | FileCheck %s

define i16 @br_ne(i16 %a, i16 %b) {
; CHECK-LABEL: br_ne NOP
; CHECK:       ; %bb.0:
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    SUBA GR7,GR6
; CHECK-NEXT:    CPA GR1,GR2
; CHECK-NEXT:    JZE LBB0_2
; CHECK-NEXT:    JUMP LBB0_1
; CHECK-NEXT:  LBB0_1 NOP
; CHECK-NEXT:    LAD GR6,1
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:    JUMP LBB0_3
; CHECK-NEXT:  LBB0_2 NOP
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ST GR6,0,GR7
; CHECK-NEXT:  LBB0_3 NOP
; CHECK-NEXT:    LD GR6,0,GR7
; CHECK-NEXT:    LD GR0,GR6
; CHECK-NEXT:    LAD GR6,2
; CHECK-NEXT:    ADDA GR7,GR6
; CHECK-NEXT:    RET
entry:
  %c = alloca i16
  %0 = icmp ne i16 %a, %b
  br i1 %0, label %then, label %else

then:
  store i16 1, i16* %c
  br label %ifret

else:
  store i16 2, i16* %c
  br label %ifret

ifret:
  %result = load i16, i16* %c
  ret i16 %result
}
