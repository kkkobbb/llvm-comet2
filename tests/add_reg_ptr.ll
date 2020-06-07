define i16 @add_reg_ptr(i16 %var1, i16* %var2_ptr) {
	%var2 = load i16, i16* %var2_ptr
	%result = add i16 %var1, %var2
	ret i16 %result
}
