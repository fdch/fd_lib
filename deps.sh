#!/bin/bash
LOGFILE="$1"
LOADLIB="lib"
SORTED=$(basename "$LOGFILE" .txt)
SORTED="$SORTED-sort.txt"
grep -e "succeeded" "$LOGFILE" | sort -u > "$SORTED"
sed -i -e 's/tried //g' "$SORTED"
sed -i -e 's/and succeeded//g' "$SORTED"
if [ -e "$SORTED-e" ]
then
  rm "$SORTED-e"
fi
if [ ! -d "$LOADLIB" ]
then
  mkdir "$LOADLIB"
fi
while read line
do
  cp -r "$line" "$LOADLIB/"
done < "$SORTED"
rm "$SORTED"
exit
