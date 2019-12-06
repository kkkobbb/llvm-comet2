; ModuleID = 'code'
source_filename = "add.ea"

define i16 @main() {
entry:
  %a = alloca i16
  store i16 1, i16* %a
  %b = alloca i16
  store i16 2, i16* %b
  %var = load i16, i16* %a
  %var1 = load i16, i16* %b
  %add_tmp = add i16 %var, %var1
  ret i16 %add_tmp
}

