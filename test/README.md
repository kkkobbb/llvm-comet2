# LLVM test

## 準備
* `ln -s ~/llvm-comet2/test/CodeGen/Comet2 $LLVM_ROOT/llvm/test/CodeGen/Comet2`
* テストには`llvm-lit`を使用するが、それ以外のツールも使用するためllvmは全てビルドする
    * `cd <build dir>`
    * `cmake --build .`

## 実行
* `python3 ./bin/llvm-lit test/CodeGen/Comet2`
    * cmakeを実行して実行ファイル等が生成されたディレクトリで実行
    * `-vv`を付けるとエラー時に詳細が出力される
