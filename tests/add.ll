; ModuleID = 'code'
source_filename = "add.ea"

define i32 @main() {
entry:
  %a = alloca i32
  store i32 1, i32* %a
  %b = alloca i32
  store i32 2, i32* %b
  %var = load i32, i32* %a
  %var1 = load i32, i32* %b
  %add_tmp = add i32 %var, %var1
  ret i32 %add_tmp
}

