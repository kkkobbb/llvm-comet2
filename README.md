# LLVM backend for the COMET II


## 準備
1. オリジナルのLLVMソースコード取得
    * `git clone https://github.com/llvm/llvm-project.git`
        * (commit d936a6301bd34ef991ff5630d06607315f34e6e7 時点のコード)
    * llvmソースコードのトップディレクトリを `$LLVM_ROOT` とする
2. ソースコード修正
    * `patches/` 内のパッチファイルを `$LLVM_ROOT` に適用する
        * `cd $LLVM_ROOT && git apply comet2.patch`
3. 追加ソースコードの配置
    * `$LLVM_ROOT/llvm/lib/Target/` に `Comet2` のシンボリックリンクを作成


## ビルド
__まだビルドできない__
* `cd $LLVM_ROOT && mkdir _build_comet2 && cd _build_comet2`
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2`
* `make llc`


## 使い方
llcのみ対応


## TODO
* とりあえずビルドを通す
    * エラーは全て修正するが、警告はできる限り修正のみ
* なるべく最小構成にする

