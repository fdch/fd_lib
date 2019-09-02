#!/bin/bash

LIST=`cat ./list.txt`

VERSION="fd_lib version 0.1"

DDIM=164
TDIMX=150
TDIMY=30
COUNT=0

for i in $LIST
do
	if  [[ $i == *"-help.pd"* ]]
	then
		VERSIONS=`cat $i | grep "$VERSION" `
		XDIM=`cat $i | grep -m 1 "#N" | cut -f5 -d' '`
		YDIM=`cat $i | grep -m 1 "#N" | cut -f6 -d' '`
		
		if [ ! "$VERSIONS" ]
		then
			TOFFX=$((XDIM-$DDIM))
			TOFFY=$((YDIM-$TDIMY))
			echo "Adding version to <$i>..."
			echo "#X text $TOFFX $TOFFY $VERSION;"  >> $i
		else
			echo $i "has already a been versioned:"
			echo "|--->" $VERSIONS
		fi
		# ((COUNT++))
	fi
	# if [ $COUNT == 3 ]
	# then
	# 	exit
	# fi
done



exit
