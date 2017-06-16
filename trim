#!/bin/bash
#
# Trim a text between delimiters
#
# 1st arg = source file
# 2nd arg = start delimiter
# 3rd arg = end delimiter (defaults to 2nd arg)
#
# For example:
# To trim what is between double quotes in file.c into path/trimmed.txt, try:
#
# $ ./trim file.c "\"" > path/trimmed.txt
#

ARG1=$1
ARG2=$2
ARG3=$3

if [ ! ARG1 ]
then
echo "No argument 1: must specify source file"
fi

if [ ! ARG2 ] 
then
echo "No argument 2: must specify delimiter start"
fi

if [ ! ARG3 ] 
then
ARG3=ARG2
echo "trim: using same delimiter to start and end"
fi

if [ ARG3 ] || [ ARG2 ] && [ ARG1 ]
then
awk -F"$ARG2" '{gsub(/$ARG3.*/,"",$2);print $2}' $ARG1 | sed '/^$/d'
fi
exit