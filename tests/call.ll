; ModuleID = 'code'
source_filename = "call.ea"

define i32 @main() {
entry:
  %0 = call i32 @callee(i32 1)
  ret i32 %0
}

define i32 @callee(i32) {
entry:
  %a = alloca i32
  store i32 %0, i32* %a
  %var = load i32, i32* %a
  ret i32 %var
}

