define i16 @br_eq(i16 %a, i16 %b) {
entry:
  %c = alloca i16
  %0 = icmp eq i16 %a, %b
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
