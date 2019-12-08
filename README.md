# LLVM backend for the COMET II


## 準備
1. オリジナルのLLVMソースコード取得
    * `git clone https://github.com/llvm/llvm-project.git`
        * (tag llvmorg-9.0.0 時点のコード)
    * llvmソースコードのトップディレクトリを `$LLVM_ROOT` とする
2. ソースコード修正
    * `patches/` 内のパッチファイルを `$LLVM_ROOT` に適用する
        * `cd $LLVM_ROOT && git apply comet2.patch`
3. 追加ソースコードの配置
    * `$LLVM_ROOT/llvm/lib/Target/` に `Comet2` のシンボリックリンクを作成


## ビルド
* `cd $LLVM_ROOT && mkdir _build_comet2 && cd _build_comet2`
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2`
* `make llc`


## 使い方
* __出力結果はほぼ適当__
* llcのみ対応
    * `llc --mtriple=comet2 tests/add.ll && gcc tests/add.s`
        * TODO target triple は正しい？
        * アセンブリ `add.s` が生成される
        * llc に `-debug` オプションを付けるとデバッグ用の情報も出力される


## TODO
* まともなコード出力をする
    * 本来はラベルのみだとエラー
    * 引数が多いとllc実行時エラー
        * スタック渡ししていない?
* ビルド時の警告修正
* なるべく最小構成にする

