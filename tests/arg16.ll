
define i16 @main() {
entry:
  %0 = call i16 @test(i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8,
                      i16 9, i16 10, i16 11, i16 12, i16 13, i16 14, i16 15, i16 16)
  ret i16 %0
}

define i16 @test(i16 %a, i16 %b, i16 %c, i16 %d, i16 %e, i16 %f, i16 %g, i16 %h,
                 i16 %i, i16 %j, i16 %k, i16 %l, i16 %m, i16 %n, i16 %o, i16 %p) {
  %ret1  = add i16 %a, %b
  %ret2  = add i16 %c, %d
  %ret3  = add i16 %e, %f
  %ret4  = add i16 %g, %h
  %ret5  = add i16 %i, %j
  %ret6  = add i16 %k, %l
  %ret7  = add i16 %m, %n
  %ret8  = add i16 %o, %p
  %ret9  = add i16 %ret1, %ret2
  %ret10 = add i16 %ret3, %ret4
  %ret11 = add i16 %ret5, %ret6
  %ret12 = add i16 %ret7, %ret8
  %ret13 = add i16 %ret9, %ret10
  %ret14 = add i16 %ret11, %ret12
  %ret15 = add i16 %ret13, %ret14
  ret i16 %ret15
}

