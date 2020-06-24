define i16 @shift_left_arith(i16 %var1, i16 %var2) {
  %result = shl i16 %var1, %var2
  ret i16 %result
}
