#!/bin/bash

# Get all methods from a pd *.c fle. Default *-method.txt is in current dir.
#
# This is not guaranteed it will work, since it's really syntax dependant.
#

if [ ! $2 ]
then 
# Use this instead of DIR="." if you want to use the same directory
#DIR=$(dirname $1)
DIR=$(pwd)
else
DIR=$2
fi

BASE=$(basename $1 .c)

METHODS=$(grep "^  class_addmethod"  $1 | awk '{/^/;print $3}' | awk -F"\"" '{gsub(/\".*/,"",$2);print $2}' | sed '/^$/d')

ARGUMENTS=$(grep "^  class_addmethod"  $1 | awk '{/^/; print $4$5$6$7$8$9$10}' |  sed '/^$/d' | sed 's/0/A_NULL/' | sed 's/\;//' | sed 's/.$//' | sed 's/.$//' | sed 's/,//' | grep -v '^$' | sed 's/A_SYMBOL,/symbol /' | sed 's/A_FLOAT,/float /' | sed 's/A_DEFSYM/defsymbol /' | sed 's/A_FLOAT/float /' | sed 's/A_SYMBOL/symbol /' | sed 's/A_NULL//' | sed 's/A_NUL//')
cd "$DIR"
paste <(echo "$METHODS") <(echo "$ARGUMENTS") > "$DIR/$BASE-methods.txt"
exit