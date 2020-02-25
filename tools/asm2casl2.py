#!/usr/bin/env python3
"""
LLVMが生成したアセンブリコードをCASL2フォーマットに変換する
"""
import argparse
import sys


def asm2casl2(inputf, outputf):
    outputf.write("LBLSTART START\n")  # FIXME 他のラベルとかぶらないようにする
    for line in inputf:
        st_line = line.strip()
        if len(st_line) == 0:
            continue
        if st_line[0] == ".":
            continue
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

