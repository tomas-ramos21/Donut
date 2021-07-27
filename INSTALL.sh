#!/bin/bash
cd ${0%/*}

echo "[Donut] Compiling"
./configure
make
echo -e "[Donut] Compilation Complete\n"
echo "You may install Donut by using \`make install\`"
