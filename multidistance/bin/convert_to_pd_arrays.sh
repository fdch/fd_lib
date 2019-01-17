#!/bin/bash

if [ -z $CONFIG ]
then
	source config.sh
fi

if [ ! -d $ARRDIR ]
then 
	mkdir $ARRDIR # Make array dir if its not there
else
	rm $ARRDIR/* # Delete its contents
fi


while read line
do
	echo $line > $ARRDIR/$ARRPREFIX-$COUNT.txt
	((COUNT++))
done < $PROBSFILE

cd $ARRDIR
for i in *
do
	cat $i | tr " " "\n" > $i-temp
	rm $i
	mv $i-temp $i
done
cd ..
