#!/bin/bash

PDMINVERSION=49-1


PD=/Applications/Pd-0.${PDMINVERSION}.app/Contents/Resources/bin/pd

if [[ $1 ]] 
then
	FILE=$1
else
	echo "usage: ./imake <filename>"
	exit 1
fi

if [[ -d $1 ]]
then
	echo "making $FILE"
else
	echo "$FILE dir doesn't exist"
	exit 1
fi

EXT=pd_darwin

make $FILE.$EXT && mv $FILE.$EXT $FILE && $PD $FILE/$FILE-help.pd

exit 0

