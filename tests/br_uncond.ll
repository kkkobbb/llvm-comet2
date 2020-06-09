define i16 @br_uncond() {
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
