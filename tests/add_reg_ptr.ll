define i16 @add_reg_ptr(i16 %val1, i16* %val2_ptr) {
	%val2 = load i16, i16* %val2_ptr
	%result = add i16 %val1, %val2
	ret i16 %result
}
