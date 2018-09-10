#!/bin/bash

grep "Placeholder"  */*-help.pd | cut -d "#" -f 1 > files_without_helpfiles.txt

FILES=`cat files_without_helpfiles`

if [[ $FILES ]] 
then
	echo "$FILES without helpfiles"
	exit 0
else
	echo "Congrats!
No files without helpfiles.
All is good with the world..."
	rm files_without_helpfiles.txt
	exit 0
fi
