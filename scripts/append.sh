#!/bin/bash

PD=/Applications/Pd-0.48-1.app/Contents/Resources/bin/pd

if [ ! `command -v pd` ]
then
	echo "Could not find 'pd', trying with default location:"
	echo $PD
	if [ ! `command -v $PD` ]
	then
		echo "Pd not found. Update line 3 with correct path."
		exit
	fi
else
	PD=pd
fi

LIST=`cat ./list.txt`


declare -a ARRAY

VERSION="fd_lib version 0.1"

DDIM=164
TDIMX=100
TDIMY=30
COUNT=0

for i in $LIST
do
	if  [[ $i == *"-help.pd"* ]]
	then
		DECLARED=`cat $i | grep "\-path fd_lib" `
		VERSIONS=`cat $i | grep "$VERSION" `
		XDIM=`cat $i | grep -m 1 "#N" | cut -f5 -d' '`
		YDIM=`cat $i | grep -m 1 "#N" | cut -f6 -d' '`
		if [ ! "$DECLARED" ]
		then
			echo "Adding declare to <$i>..."
			DOFF=$((XDIM-$DDIM))
			echo "#X obj $DOFF 15 declare -lib fd_lib;"
			echo "#X obj $DOFF 15 declare -lib fd_lib;" >> $i
		else
			echo $i "has already fd_lib declared:"
			echo "|--->" $DECLARED
		fi
		if [ ! "$VERSIONS" ]
		then
			TOFFX=$((XDIM-$TDIMX))
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
