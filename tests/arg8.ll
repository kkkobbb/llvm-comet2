
define i16 @main() {
entry:
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d, i16 %e, i16 %f, i16 %g, i16 %h) {
    %ret1 = add i16 %a, %b
    %ret2 = add i16 %c, %d
    %ret3 = add i16 %e, %f
    %ret4 = add i16 %g, %h
    %ret5 = add i16 %ret1, %ret2
    %ret6 = add i16 %ret3, %ret4
    %ret7 = add i16 %ret5, %ret6
    ret i16 %ret7
}

