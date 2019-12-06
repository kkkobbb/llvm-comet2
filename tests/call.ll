; ModuleID = 'code'
source_filename = "call.ea"

define i16 @main() {
entry:
  %0 = call i16 @callee(i16 1)
  ret i16 %0
}

define i16 @callee(i16) {
entry:
  %a = alloca i16
  store i16 %0, i16* %a
  %var = load i16, i16* %a
  ret i16 %var
}

