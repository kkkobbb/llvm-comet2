# LLVMバックエンドメモ

## ターゲット追加時のビルド

### cmake
* `cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2 -DLLVM_DEFAULT_TARGET_TRIPLE=comet2-unknown-unknown`
    * `-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2`
        * 試験的なターゲットの場合、このオプションで指定する必要がある
            * 公式にあるターゲットでも必要な場合がある
        * 自分で追加した場合はこのオプションを指定する
        * `Target/` に配置したディレクトリ名を指定する
        * このオプションを指定すれば、`MakeLists.txt` `LLVMBuild.txt`は変更しなくていい？
    * `-DLLVM_TARGETS_TO_BUILD=Comet2`
        * このオプションで指定したマシンのみビルド対象にする
        * `Target/` に配置したディレクトリ名を指定する
        * 複数指定する場合は `;` で区切る (その場合、`'`で囲わないとshが`;`を処理してしまう)
            * 例 `-DLLVM_TARGETS_TO_BUILD='RISCV;Comet2'`
    * `-DLLVM_DEFAULT_TARGET_TRIPLE=comet2-unknown-unknown`
        * デフォルトのターゲットトリプルを変更するオプション
        * 指定なしの場合、ホストマシンと同じ値となる
    * 全てビルドするとかなり時間がかかるので一部だけビルドする
        * `cmake --build . --target llc` or `make llc`
        * `llc`のみなら4GBでもビルドは成功する

### Target以外のコード修正
* `$LLVM_ROOT/llvm/include/llvm/ADT/Triple.h`
* `$LLVM_ROOT/llvm/lib/Support/Triple.cpp`
* `$LLVM_ROOT/llvm/lib/Target/LLVMBuild.txt`

### ビルド時のエラー
* `llvm::CGIOperandList::getSubOperandNumber(unsigned int) const: Assertion i < OperandList.size() && "Invalid flat operand #"' failed.`
    * 親classで定義した全ての変数に値が設定されていない場合でるっぽい

### 実行時
* `./bin/llc --march=comet2 -debug`
    * `--march` 出力するasmのアーキテクチャを指定する
    * `-debug` デバッグ情報を表示する

## 開発環境
* vim用の llvm関係シンタックスハイライト定義ファイル
    * `$LLVM_ROOT/llvm/utils/vim/syntax/tablegen.vim`
        * バックエンド記述の `*.td` ファイル用
    * `$LLVM_ROOT/llvm/utils/vim/syntax/llvm.vim`
        * llvm中間表現 `*.ll` ファイル用
* 使い方
    * `~/.vim/syntax`に定義ファイルを置く
    * `$LLVM_ROOT/llvm/utils/vim/vimrc`を参考に`~/.vimrc`を修正する


## Target/ での記述で参考になりそうな部分
* `ProcessorModel`
    * `SubtargetFeature`を置き換える新しいクラス?
    * 参考 `$LLVM_ROOT/llvm/include/llvm/Target/Target.td`
* 以下の2か所を変更すると`.text`セクションを変更できる
    * `$LLVM_ROOT/llvm/lib/MC/MCObjectFileInfo.cpp` +343
    * `$LLVM_ROOT/llvm/lib/CodeGen/TargetLoweringObjectFileImpl.cpp` +611
* 最後にコード追加
    * `$LLVM_ROOT/llvm/include/llvm/MC/MCStreamer.h` +117 finish()
    * 参考 `$LLVM_ROOT/llvm/lib/Target/AVR/MCTargetDesc/AVRTargetStreamer.cpp` +24


## LLVM 概要
* 処理の流れ
    * IR -> SelectionDAG -> MachineDAG -> MachineInstr -> MCInst


## llvm共通部分のコード
* `Tied` 2つのオペランドが同じレジスタにマッピングされたことを表す
    * `$LLVM_ROOT/llvm/include/llvm/MC/MCInstrDesc.h` `TIED_TO`
* 同じレジスタへのマッピング関係の処理？
    * `$LLVM_ROOT/llvm/lib/CodeGen/TwoAddressInstructionPass.cpp` `tryInstructionTransform`
* SelectionDAGノードのリスト、説明
    * `$LLVM_ROOT/llvm/include/llvm/CodeGen/ISDOpcodes.h`


## Comet2での実装のメモ

### ASM出力時のフォーマット調整
* `Comet2/MCTargetDesc/Comet2MCAsmInfo.cpp`
* 変更可能な部分の例
    * コメントを表す文字
    * ラベルの末尾の文字
    * アラインメントの有無
    * デバッグ情報の有無
    * 一部のディレクティブの有無

### アセンブリの記述
* `Comet2/Comet2InstrInfo.td`
    * 各命令の動作記述
* `Comet2/Comet2InstrFormats.td`
    * 命令のフォーマット
* `Comet2/Comet2RegisterInfo.td`
    * レジスタの定義
* `Comet2/Comet2InstrInfo.td`
    * tdファイルで記述できない部分の処理
    * レジスタの一時退避時のコード等

### LLVM IRにアーキテクチャ固有の変換を実施
* `Comet2/Comet2ISelLowering.cpp`
    * 関数呼び出し時の引数の扱い等
* `Comet2/Comet2FrameLowering.cpp`
    * 関数呼び出し時のスタックの処理等
    * prologue、epilogue処理の記述もここ

### 分岐命令の実装について
* Comet2の分岐命令はフラグレジスタの状態で判断する
* (brcond (seteq) bb:$dst)を(JZE (CPA) bb:$dst)に変換することで対応している
    * この変換をするために、CPAの仮想的な出力$condをJZEの仮想的な入力$condとしている
    * brcond が そのオペランド(seteq setne等)によって変化するため直接brcondの変換をしていない

### setOperationAction()について
* LLVM IRの変換を制御する
* `setOperationAction(ISD::BR_CC, MVT::i16, Expand)`の場合
    * (たぶん) デフォルトでは `brcond`、`set*`の2つがあれば結合して`br_cc`を生成している
    * `setOperationAction`で`BR_CC`に`Expand`を指定した場合、`br_cc`を生成する処理をしなくなる


## debugメッセージ
* `ISEL: Starting pattern match`でのindex番号は以下参考
    * `ビルド用ディレクトリ/lib/Target/Comet2/Comet2GenDAGISel.inc` `MatcherTable`


## 参考
* 公式のバックエンドのソースコード `$LLVM_ROOT/llvm/lib/Target/*`
    * 特に`RISCV`、`Sparc`、`AVR`
* きつねさん
