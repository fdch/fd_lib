#!/bin/bash
CFILE="$1"
if [[  $CFILE  ]] 
then
make "$CFILE.pd_darwin" && mv "$CFILE.pd_darwin" "$CFILE" && open "$CFILE/$CFILE-help.pd"
else
echo "No target c file name specified"
fi
exit
