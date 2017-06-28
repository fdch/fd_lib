#!/bin/bash
FILENAME="!an_overview/externals-overview.pd"
OBJ=$(basename $(find . -name "*.pd_darwin" -print | sed 's/.pd_darwin//' | sed '/^$/d'))
echo "#N canvas 0 22 450 300 12;" > "$FILENAME"
YPOS=25
for i in $OBJ
do
echo "#X obj 10 $YPOS $i;" >> "$FILENAME"
((YPOS+=25))
done
exit