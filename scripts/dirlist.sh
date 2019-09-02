#!/bin/bash
DIRLIST=""
TARGET=list.txt
#	Iterate through all non-help or -test files
DIRLIST=$(find . ! -name "*-help.pd" ! -name "*-test.pd" ! -name "_*.pd" -name "*.pd" -maxdepth 1 | sed -e 's/.pd//g')
for i in $DIRLIST
do
	PATCHFILE="$i.pd"
	HELPFILE="$i-help.pd"
	TESTFILE="$i-test.pd"
	PLACEHOLDER=$(echo "
#N canvas 0 22 450 350 14;
#X text 10 10 $i;
#X text 10 40 Placeholder for help file:;
#X obj 10 70 $i;
#X text 350 300 fd_lib v0.1;
")
	PLACEHOLDERREADME=$(echo "
# $i 
part of fd_lib
.
")
	#	Check first that directory doesn't exist
	if [ ! -d $i ]
	then 
		
		#	Create directory with the same name as the non-help filename
		mkdir $i
		
		#	Put Patch file, Placeholder and Readme in it
		echo $PLACEHOLDER > "$i/$HELPFILE"
		echo $PLACEHOLDERREADME > "$i/README.md"
		mv $PATCHFILE $i
		
		#	Replace the placeholder with an existing help file
		mv $HELPFILE $i
		
		#	put testfiles in there as well if there are any
		mv $TESTFILE $i
	elif [ ! -f "$i/$PATCHFILE" ]
	then
		mv $PATCHFILE $i 
	elif [ ! -f "$i/$HELPFILE" ]
	then
		#	Create placeholder only if there isn't a help file already there
		echo $PLACEHOLDER > "$i/$HELPFILE"
	elif [[ ! -f "$i/README.md" ]]
	then
		echo $PLACEHOLDERREADME > "$i/README.md"
	fi
done

#	Update $TARGET with all Pd files in this directory and any subdirectory.
find . -name "*.pd" > $TARGET
cat $TARGET | sort > /tmp/sorted
cat /tmp/sorted > $TARGET
COUNT=`sed -n '$=' $TARGET`
echo "Wrote $COUNT files in $TARGET"
echo "Now run sh fdlib ..."

exit