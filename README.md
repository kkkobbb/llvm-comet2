# LLVM backend for the COMET II


## 準備
1. オリジナルのLLVMソースコード取得
    * `git clone https://github.com/llvm/llvm-project.git`
    * llvmソースコードのトップディレクトリを `$LLVM_ROOT` とする
2. ソースコード修正
    * `cd $LLVM_ROOT`
    * `git checkout llvmorg-9.0.0`
        * 9.0.0のバージョンにする
    * `git apply ~/llvm-comet2/patches/comet2.patch`
        * `patches/` 内のパッチファイルを `$LLVM_ROOT` に適用する
3. 追加ソースコードの配置
    * `ln -s ~/llvm-comet2/Comet2 $LLVM_ROOT/llvm/lib/Target/Comet2`
        * `Target/` に `Comet2/` のシンボリックリンクを作成


## ビルド
* `cd $LLVM_ROOT && mkdir _build_comet2 && cd _build_comet2`
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2 -DLLVM_DEFAULT_TARGET_TRIPLE=comet2-unknown-unknown`
* `cmake --build . --target llc`


## 使い方
* llcのみ対応
    * `llc --march=comet2 tests/add_reg_reg.ll`
        * アセンブリ `add_reg_reg.s` が.llファイルと同じ場所に生成される
        * `--march=comet2`はなくてもいい
    * そのままのアセンブリでは文法違反
        * `.text` の行を削除
        * 先頭行に`LABEL START`を追加 (ラベルは他と被らないもの)
        * 最終行に` END`を追加
        * `tools/s2casl2.py`で変換できる


## TODO
* まともなコード出力をする
    * 本来メモリは2byte単位だがそうなっていない
        * GR0 GR1の値を0x1000から連続でstoreする場合、0x1000 0x1001でいいはずが、 0x1000 0x1002にする
        * とりあえずメモリを無駄に2倍使うだけで動作はするはず
* ビルド時の警告修正
* なるべく最小構成にする
    * tdファイル（特に命令フォーマット等）の整理
    * 不要なファイル、関数はある？
    * `Disassembler/` は必要？

