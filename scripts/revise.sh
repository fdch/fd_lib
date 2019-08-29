#!/bin/bash

PD=/Applications/Pd-0.48-1.app/Contents/Resources/bin/pd

LIST=`cat ../list.txt`


declare -a ARRAY


for i in $LIST
do
	ARRAY+=( "${arr[@]}" "$i" )
done


OFFSET=$1
ARROFFSET=`echo "${ARRAY[@]:$OFFSET}"`

COUNT=$OFFSET

for j in $ARROFFSET
do
	echo "--------- [$COUNT] [$j] ----------"
	$PD $j
	((COUNT++))
	echo "--------- [end] [$COUNT] ---------"
done

exit 0