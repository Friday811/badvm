#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Convert a text file to bytes"""
# pylint: disable=invalid-name

import sys
data = ''
op_pairs = {
    "0": "NOOP",
    "1": "LOAD",
    "2": "ADD ",
    "3": "SUB ",
    "4": "CMP ",
    "5": "CHNG",
    "6": "PUSH",
    "7": "POP ",
    "8": "JMPL",
    "9": "JMPG",
    "A": "JMPE",
    "B": "JMPU",
    "C": "READ",
    "D": "PRNS",
    "E": "PRNT",
    "F": "HALT"
}

with open(sys.argv[1], "r") as f:
    data = "".join(line.rstrip()[0:4] for line in f)
    if len(data) % 4 != 0:
        print("Error: input file contains incomplete or malformed operations")
        sys.exit(1)
    print("-----   PROGRAM DUMP   -----")
    print("INS   REG1  REG2  REG3   IMM")
    print("----  ----  ----  ----  ----")
    for i in range(0, len(data), 4):
        print(f"{op_pairs[data[i]]}   r{data[i+1]}    r{data[i+2]}"
              f"    r{data[i+3]}    #{data[i+2:i+4]}")
    data = bytearray.fromhex(data)
    data[0::2], data[1::2] = data[1::2], data[0::2]  # swap word endianness

with open(sys.argv[2], "wb") as f:
    f.write(data)
