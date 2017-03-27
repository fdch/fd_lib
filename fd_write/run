#!/bin/bash
MAIN=$(pwd)
FULLPATH=$1
FILENAME=$2
LENGTH=$3
if [$FULLPATH]; then
FULLPATH=$1
else
FULLPATH=$MAIN
fi
if [$FILENAME]; then
FILENAME=$2
else
FILENAME="file"
fi
if [$LENGTH]; then
LENGTH=$3
else
LENGTH="0.1"
fi
PDPATH=/Applications/Pd-0.47-1.app/Contents/Resources/bin/
echo "
**********
Pd path: $PDPATH
arguments (optional):
1 - full-path (recording directory defaults to $MAIN)
2 - filename (filename defaults to "file")
3 - length (length in minutes defaults to 0.1)
*********
"
cd $PDPATH
./pd -nogui -send ";full-path $FULLPATH;length $LENGTH;filename $FILENAME;" $MAIN/fd_write.pd
cd $MAIN
exit
