#!/bin/bash
#Iterate through all non-help files
DIRLIST=$(find . ! -name "*-help.pd" -name "*.pd" -maxdepth 1 | sed -e 's/.pd//g')
for i in $DIRLIST; do
PATCHFILE="$i.pd"
HELPFILE="$i-help.pd"
PLACEHOLDER=$(echo "
#N canvas 0 22 450 350 14;
#X text 10 10 $i;
#X text 10 40 Placeholder for help file:;
#X obj 10 70 $i;
#X text 400 300 fd_lib;
")
PLACEHOLDERREADME=$(echo "
#README 
$i is part of fd_lib
")
#Check first that directory doesn't exist
if [ ! -d $i ]; then 
	#Create directory with the same name as the non-help filename
	mkdir $i
	#Put Patch file, Placeholder and Readme in it
	echo $PLACEHOLDER > "$i/$HELPFILE"
	echo $PLACEHOLDERREADME > "$i/README.md"
	mv $PATCHFILE $i
	#Replace the placeholder with an existing help file
	mv $HELPFILE $i
elif [ ! -f "$i/$PATCHFILE" ]; then
	mv $PATCHFILE $i 
elif [ ! -f "$i/$HELPFILE" ]; then
	#Create placeholder only if there isn't a help file already there
	echo $PLACEHOLDER > "$i/$HELPFILE"
elif [[ ! -f "$i/README.md" ]];  then
	echo $PLACEHOLDERREADME > "$i/README.md"
fi
done
#Update list.txt with all Pd files in this directory and any subdirectory.
find . -name "*.pd" > list.txt
echo "
Now run ./fdlib ..."
DIRLIST=""
exit