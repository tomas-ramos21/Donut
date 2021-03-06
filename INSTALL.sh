#!/bin/bash
cd ${0%/*}

echo "[Donut] Running Autotools"
autoconf
./configure
echo "[Donut] Running Makefile"
make
make install
