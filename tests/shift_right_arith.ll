define i16 @shift_right_arith(i16 %var1, i16 %var2) {
  %result = ashr i16 %var1, %var2
  ret i16 %result
}
