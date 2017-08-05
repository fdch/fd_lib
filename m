#!/bin/bash
CFILE="$1"
PDDIRECTORY="/usr/local/include/pd/"
if [[  $CFILE  ]] 
then
echo "
making with pd headers at $PDDIRECTORY"
make "$CFILE.pd_darwin" PDDIR=$PDDIRECTORY && mv "$CFILE.pd_darwin" "$CFILE" && open "$CFILE/$CFILE-help.pd" 
else
echo "No target c file name specified"
fi
exit
