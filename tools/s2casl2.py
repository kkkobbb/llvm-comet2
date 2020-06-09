#!/usr/bin/env python3
"""
LLVMが生成したアセンブリコードをCASL2フォーマットに変換する
"""
import argparse
import sys
import re

RE_LABEL_LINE = re.compile(r"^[a-zA-Z0-9._].+")
RE_BAD_LABEL_CHAR = re.compile(r"[._]")


def asm2casl2(inputf, outputf):
    outputf.write("LSTART  START\n")  # FIXME 他のラベルとかぶらないようにする
    for line in inputf:
        st_line = line.strip()
        if len(st_line) == 0:
            continue
        is_comment = st_line[0] == ";"
        is_label = (not is_comment) and (re.search(RE_LABEL_LINE, line) is not None)
        is_directive = (not is_label) and st_line[0] == "."
        if is_directive:
            continue
        if not is_comment:
            # ラベルとして使用できない文字は削除する
            # FIXME 末尾のコメント中の文字も削除してしまう
            line = re.sub(RE_BAD_LABEL_CHAR, "", line)
        outputf.write(line)
    outputf.write(" END\n")


def main():
    parser = argparse.ArgumentParser(
            description=__doc__,
            formatter_class=argparse.RawDescriptionHelpFormatter)

    args = parser.parse_args()

    asm2casl2(sys.stdin, sys.stdout)

if __name__ == "__main__":
    main()

