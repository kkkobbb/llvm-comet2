# LLVMバックエンドメモ

## ターゲット追加時のビルド

### cmake
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2`
    * `-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2`
        * 自分で追加した場合はこのオプションを指定する
        * `Target/` に配置したディレクトリ名を指定する
    * `-DLLVM_TARGETS_TO_BUILD=Cpu0`
        * このオプションで指定したマシンのみビルド対象にする
        * `Target/` に配置したディレクトリ名を指定する

### コード修正
* `$LLVM_ROOT/llvm/include/llvm/ADT/Triple.h`
* `$LLVM_ROOT/llvm/lib/Support/Triple.cpp`


## その他
* `ProcessorModel`
    * `SubtargetFeature`を置き換える新しいクラス?
    * 参考 `$LLVM_ROOT/llvm/include/llvm/Target/Target.td`


## 参考
* 公式のバックエンドのソースコード `$LLVM_ROOT/llvm/lib/Target/*`
    * `RISCV`、`Sparc`を参考にした


