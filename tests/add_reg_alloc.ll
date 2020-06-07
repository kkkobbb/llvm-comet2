define i16 @add_reg_alloc(i16 %var1) {
  %a = alloca i16
  store i16 1, i16* %a
  %var2 = load i16, i16* %a
  %result = add i16 %var1, %var2
  ret i16 %result
}
