#!/bin/bash
#
#
#	This file sorts all probs into $SORTEDPROBS and 
#	their indices into $SORTEDID
#
#

if [ -z $CONFIG ]
then
	source config.sh
fi

if [ $1 ]
then
	DIRECTION="-r"
else
	DIRECTION=""
fi

#	Count number of fields in probsfile
COLS=`head -n 1 $PROBSFILE | awk '{print NF}'`

#	Prepare a new file for probs with an added ID at the end
#	Which is not taken into account in the sorting
echo "" > probs_temp
i=0
while read line
do
	echo "$line ID=$i" >> probs_temp
	((i++))

done < $PROBSFILE

#	Remove the first empty line in probs_temp
sed -ie '/^$/d' probs_temp

#	Make the sort flag that includes all fields for sorting
i=1
while [ "$i" -le "$COLS" ]
do
  SORTFLAG+=`echo " -k$i"`
  i=$(($i + 1))
done

#	Do the sorting into $SORTEDPROBS
sort $DIRECTION -n $SORTFLAG	probs_temp -o $SORTEDPROBS
	
#	Extract IDS to SORTEDID and probs to SORTEDPROBS 
cat $SORTEDPROBS | cut -d= -f2 > $SORTEDID
cat $SORTEDPROBS | cut -dI -f1 > $SORTEDPROBS-temp
mv $SORTEDPROBS-temp $SORTEDPROBS
#	Get rid of temp file
rm probs_temp*

exit 
