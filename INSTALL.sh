#!/bin/bash
cd ${0%/*}

echo "[Donut] Running Autotools"
./configure
echo "[Donut] Running Makefile"
make
make install
make clean
