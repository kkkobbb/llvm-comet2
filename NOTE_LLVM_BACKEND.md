# LLVMバックエンドメモ

## ターゲット追加時のビルド

### cmake
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2`
    * `-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2`
        * 自分で追加した場合はこのオプションを指定する
        * `Target/` に配置したディレクトリ名を指定する
        * このオプションを指定すれば、`MakeLists.txt` `LLVMBuild.txt`は変更しなくていい？
    * `-DLLVM_TARGETS_TO_BUILD=Comet2`
        * このオプションで指定したマシンのみビルド対象にする
        * `Target/` に配置したディレクトリ名を指定する
        * 複数指定する場合は `;` で区切る
    * 全てビルドするとかなり時間がかかるので一部だけビルドする
        * `make llc`

### コード修正
* `LLVM_EXPERIMENTAL_TARGETS_TO_BUILD`を指定すれば不要？
* `$LLVM_ROOT/llvm/include/llvm/ADT/Triple.h`
* `$LLVM_ROOT/llvm/lib/Support/Triple.cpp`


## その他
* `ProcessorModel`
    * `SubtargetFeature`を置き換える新しいクラス?
    * 参考 `$LLVM_ROOT/llvm/include/llvm/Target/Target.td`
* vim用の llvm関係シンタックスハイライト定義ファイル
    * `$LLVM_ROOT/llvm/utils/vim/syntax/tablegen.vim`
        * バックエンド記述の `*.td` ファイル用
    * `$LLVM_ROOT/llvm/utils/vim/syntax/llvm.vim`
        * llvm中間表現 `*.ll` ファイル用

## ビルド時のエラー
* `llvm::CGIOperandList::getSubOperandNumber(unsigned int) const: Assertion i < OperandList.size() && "Invalid flat operand #"' failed.`
    * 親classで定義した全ての変数に値が設定されていない場合でるっぽい


## 参考
* 公式のバックエンドのソースコード `$LLVM_ROOT/llvm/lib/Target/*`
    * 特に`RISCV`、`Sparc`
* きつねさん

