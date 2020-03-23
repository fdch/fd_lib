#!/bin/bash
#
# This script loads a pd patch (argument 1) and puts all dependencies in the 
# directory specified with argument 2
#
if [[ -f /usr/local/bin/pd ]]
then 
	MYPD=/usr/local/bin/pd
elif [[ -f /usr/bin/pd ]]
then
	MYPD=/usr/bin/pd
elif [[ -f /Applications/Pd-0.50-0.app/Contents/Resources/bin/pd ]]
then
	MYPD=/Applications/Pd-0.50-0.app/Contents/Resources/bin/pd
else
	echo "Could not find Pd. Please provide full/path/to/bin/pd"
	exit 1
fi
# echo using pd in $MYPD

#get pdfile name from argument 1
if [ "$1" ]
then
	PDFILE="$1"
else
	echo "Please provide a pd patch in argument 1"
	exit 1
fi
# echo using $PDFILE
#get libdir name from argument 2
if [ "$2" ]
then
	LIBDIR="$2"
else
	echo "Please provide a library directory name in argument 2"
	exit 1
fi
# echo exporting all in $LIBDIR
#get logfile name from pd file
LOGFILE=$(basename $PDFILE .pd)-log

# cat $LOGFILE
#load pd file and output console printout to logfile
echo "
waiting 5 seconds...
"

$($MYPD -stderr -verbose -open $PDFILE -nogui  2> "$LOGFILE") & 
sleep 5
kill $!
wait 2>/dev/null

SORTED=$(basename "$LOGFILE" .txt)
SORTED="$SORTED-sort.txt"

# cat $SORTED

grep -e "succeeded" "$LOGFILE" | sort -u > "$SORTED"
sed -i -e 's/tried //g' "$SORTED"
sed -i -e 's/and succeeded//g' "$SORTED"

# remove unwanted sorted text

if [ -e "$SORTED-e" ]
then
	# echo removing $SORTED-e
  	rm "$SORTED-e"
fi

# make the lib directory

if [ ! -d "$LIBDIR" ]
then
	# echo making $LIBDIR
  	mkdir "$LIBDIR"
fi

# read the text and copy all files into the lib directory

while read line
do
  cp -r "$line" "$LIBDIR/"
done < "$SORTED"

# echo removing log files
rm "$SORTED"
rm "$LOGFILE"
# echo done. exiting.

tree $LIBDIR
exit