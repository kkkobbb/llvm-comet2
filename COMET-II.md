# COMET II 仕様

## 機械語
* 1ワード16ビット
* リトルエンディアンとする

### レジスタ
* `GR0` - `GR7`
    * 汎用レジスタ
    * GR0はインデックスレジスタに使用できない
    * 独自仕様
        * GR0: 戻り値を格納
        * GR6: スタックポインタ計算用
        * GR7: 関数用のスタックポインタ（フレームレジスタ）
* `SP`
    * スタックポインタ
    * スタックの伸びる向きはとりあえず負にしている
* `PR`
    * プログラムカウンタ
* `FR`
    * フラグレジスタ
    * 以下の3つ
        * OF(Overflow Flag)
            * 演算結果がオーバーフローした場合1、それ以外は0
        * SF(Sign Flag)
            * 演算結果が負の場合1、それ以外は0
        * ZF(Zero Flag)
            * 演算結果が0の場合1、それ以外は0

### Instruction Format

#### 1ワード長
* レジスタのみの命令
* 8bit  opcode
* 4bit  operand1
    * `r1`
    * `GR0` -> `0b0000`, `GR1` -> `0b0001` ...
* 4bit  operand2
    * `r2`

```
           8bit               4bit         4bit
|------------------------|------------|------------|
          opcode            operand1     operand2
```


#### 2ワード長
* アドレスを指定する命令
* 8bit  opcode
* 4bit  operand1
    * `r`
* 4bit  operand3
    * `x`
    * 0の場合、指定なし
        * よってGR0は指定できない
* 16bit operand2
    * `adr`

```
           8bit               4bit         4bit
|------------------------|------------|------------|
          opcode            operand1     operand3

                       16bit
|--------------------------------------------------|
                      operand2
```

### オペランド
* `r` `r1` `r2`
    * レジスタ (`GR0` - `GR7`)
* `adr`
    * アドレス値
* `x`
    * インデックスレジスタ (`GR1` - `GR7`)

### no operation
* `NOP`
    * `0x00 0x00`
    * 何もしない

### load/store
* `LD   r, adr [,x] (set FR)`
    * `0x10 0xhh 0xhh 0xhh`
    * `r <- mem(adr + x)`
* `ST   r, adr [,x]`
    * `0x11 0xhh 0xhh 0xhh`
    * `mem(adr + x) <- r`
* `LAD  r, adr [,x]`
    * `0x12 0xhh 0xhh 0xhh`
    * `r <- adr + x`
* `LD   r1, r2 (set FR)`
    * `0x14 0xhh`
    * `r1 <- r2`

### calc instruction
* `ADDA r, adr [,x]`
    * `0x20 0xhh 0xhh 0xhh`
    * `r <- r + mem(adr + x) (set FR)`
    * 算術加算(符号付きとみなす)
* `SUBA r, adr [,x]`
    * `0x21 0xhh 0xhh 0xhh`
    * `r <- r - mem(adr + x) (set FR)`
* `ADDL r, adr [,x]`
    * `0x22 0xhh 0xhh 0xhh`
    * `r <- r + mem(adr + x) (set FR)`
    * 論理加算(符号無しとみなす)
* `SUBL r, adr [,x]`
    * `0x23 0xhh 0xhh 0xhh`
    * `r <- r - mem(adr + x) (set FR)`
* `ADDA r1, r2`
    * `0x24 0xhh`
    * `r1 <- r1 + r2 (set FR)`
* `SUBA r1, r2`
    * `0x25 0xhh`
    * `r1 <- r1 - r2 (set FR)`
* `ADDL r1, r2`
    * `0x26 0xhh`
    * `r1 <- r1 + r2 (set FR)`
* `SUBL r1, r2`
    * `0x27 0xhh`
    * `r1 <- r1 - r2 (set FR)`

### bit instruction
* `AND  r, adr [,x]`
    * `0x30 0xhh 0xhh 0xhh`
    * `r <- r and mem(adr + x) (set FR)`
* `OR   r, adr [,x]`
    * `0x31 0xhh 0xhh 0xhh`
    * `r <- r or mem(adr + x) (set FR)`
* `XOR  r, adr [,x]`
    * `0x32 0xhh 0xhh 0xhh`
    * `r <- r xor mem(adr + x) (set FR)`
* `AND  r1, r2`
    * `0x34 0xhh`
    * `r1 <- r1 and r2 (set FR)`
* `OR   r1, r2`
    * `0x35 0xhh`
    * `r1 <- r1 or r2 (set FR)`
* `XOR  r1, r2`
    * `0x36 0xhh`
    * `r1 <- r1 xor r2 (set FR)`

### compare instruction
* `CPA  r, adr [,x]`
    * `0x40 0xhh 0xhh 0xhh`
    * `r - mem(adr + x) (set FR)`
* `CPL  r, adr [,x]`
    * `0x41 0xhh 0xhh 0xhh`
    * `r - mem(adr + x) (set FR)`
* `CPA  r1, r2`
    * `0x44 0xhh`
    * `r1 - r2 (set FR)`
* `CPL  r1, r2`
    * `0x45 0xhh`
    * `r1 - r2 (set FR)`

### shift instruction
* `SLA  r, adr [,x]`
    * `0x50 0xhh 0xhh 0xhh`
    * `r <- r << mem(adr + x) (set FR)`
* `SRA  r, adr [,x]`
    * `0x51 0xhh 0xhh 0xhh`
    * `r <- r >> mem(adr + x) (set FR)`
* `SLL  r, adr [,x]`
    * `0x52 0xhh 0xhh 0xhh`
    * `r <- r << mem(adr + x) (set FR)`
* `SRL  r, adr [,x]`
    * `0x53 0xhh 0xhh 0xhh`
    * `r <- r >> mem(adr + x) (set FR)`

### jump instruction
* `JMI  adr [,x]`
    * `0x61 0xhh 0xhh 0xhh`
    * `PR <- adr + x  if SF = 1`
    * 負数の場合、ジャンプ
* `JNZ  adr [,x]`
    * `0x62 0xhh 0xhh 0xhh`
    * `PR <- adr + x  if ZF = 0`
    * 非ゼロの場合、ジャンプ
* `JZE  adr [,x]`
    * `0x63 0xhh 0xhh 0xhh`
    * `PR <- adr + x  if ZF = 1`
    * ゼロの場合、ジャンプ
* `JUMP adr [,x]`
    * `0x64 0xhh 0xhh 0xhh`
    * `PR <- adr + x`
    * 無条件分岐
* `JPL  adr [,x]`
    * `0x65 0xhh 0xhh 0xhh`
    * `PR <- adr + x  if SF = 0, ZF = 0`
    * 正数の場合、ジャンプ
* `JOV  adr [,x]`
    * `0x66 0xhh 0xhh 0xhh`
    * `PR <- adr + x  if OF = 1`
    * オーバーフロー発生の場合、ジャンプ

### stack instruction
* `PUSH adr [,x]`
    * `0x70 0xhh 0xhh 0xhh`
    * `mem(SP) <- adr + x, SP--`
* `POP  r`
    * `0x71 0xhh`
    * `r <- mem(SP), SP++`

### call instruction
* `CALL adr [,x]`
    * `0x80 0xhh 0xhh 0xhh`
    * `mem(SP) <- PR, SP--, PR <- adr + x`
* `RET`
    * `0x81 0x00`
    * `PR <- mem(SP) + 1, SP++`

### other
* `SVC  adr [,x]`
    * `0xf0 0xhh 0xhh 0xhh`
    * Supervisor Call
    * 処理系依存の処理 (GR FR含む)
    * アセンブラのIN/OUT命令等で使用


## アセンブラ

### 文法
* `LABEL OPCODE OPERAND ...`
    * 先頭に文字列があるとラベル
* ` OPCODE OPERAND ...`
    * 先頭に空白文字列があるとラベルなし
* `; abcd`
    * `;`以降はコメント

### アセンブラ命令
* `START LBL`
    * プログラムの開始位置をラベルLBLにする
    * LBLを省略するとここから開始する
    * この命令の位置を示すラベルは必須
* `END`
    * プログラムの終了位置
* `DS n`
    * n語分の領域を確保する
    * nは0でも可能
        * IPAのシミュレータは実行可能
        * 一部のシミュレータではエラーになる
* `DC n [, n]`
    * 定数を格納する
        * 複数種類複数個指定可能
    * nは10進定数、16進定数、文字列、アドレスのいずれか
        * `d` 10進定数は16ビットに収まらない場合、下位16ビットを格納する
        * `#hhhh` 16進定数は4桁の16進数で指定する
        * `'string'` 文字列は`'`で囲んだ_図形文字_で指定する 文字列中で`'`を使う場合`''`とする
            * エスケープは`''`のみ
        * `label` アドレスはラベルで指定する
    * 複数ワードになる場合は連続して格納する
* `IN LBL1 LBL2`
    * LBL1以降に入力した文字列を格納し、LBL2に文字数を格納する (1文字1ワード)
* `OUT LBL1 LBL2`
    * LBL1以降のLBL2に格納された数分を文字列として表示する (1文字1ワード)
* `RPUSH`
    * GR1, GR2 ...の順番でスタックに格納する
* `RPOP`
    * スタックの内容を上からGR7, GR6 ...の順に格納する

### オペランド
* `GR0` - `GR7`
    * 汎用レジスタ
* `#hhhh`
    * 16進数アドレス
* `n`
    * 10進数アドレス
* `'abcd'`
    * 文字列
* `=n`
    * 10進数即値
    * どこかの領域にnを格納し、格納したアドレスが実際の値となる

