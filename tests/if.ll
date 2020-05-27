; ModuleID = 'code'

define i16 @main() {
entry:
  %a = alloca i16
  store i16 0, i16* %a
  %b = alloca i16
  store i16 0, i16* %b
  %var = load i16, i16* %a
  %lt_tmp = icmp slt i16 %var, 2
  %0 = zext i1 %lt_tmp to i16
  %1 = icmp ne i16 %0, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  store i16 1, i16* %b
  br label %ifret

else:                                             ; preds = %entry
  store i16 2, i16* %b
  br label %ifret

ifret:                                            ; preds = %else, %then
  %2 = phi i16 [ 1, %then ], [ 2, %else ]
  ret i16 %2
}

