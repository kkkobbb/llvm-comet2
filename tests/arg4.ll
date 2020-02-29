
define i16 @main() {
entry:
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d) {
    %ret1 = add i16 %a, %b
    %ret2 = add i16 %c, %d
    %ret5 = add i16 %ret1, %ret2
    ret i16 %ret5
}

