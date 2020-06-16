define i16 @bit_op_and(i16 %a, i16 %b) {
  %result = and i16 %a, %b
  ret i16 %result
}

define i16 @bit_op_or(i16 %a, i16 %b) {
  %result = or i16 %a, %b
  ret i16 %result
}

define i16 @bit_op_xor(i16 %a, i16 %b) {
  %result = xor i16 %a, %b
  ret i16 %result
}
