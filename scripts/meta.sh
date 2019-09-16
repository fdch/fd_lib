#!/bin/bash
#
# 	update fd_lib .meta files
# 	
#
source scripts/absmap.sh || source absmap.sh

echo "updating list.txt..."
sh dirlist # update list.txt

echo "parsing files..."
COUNT=0
while read line
do
	if grep -q "fd_" <<< $line ## only accept 'fd_'-type files
	then
		if grep -q -v "-help.pd" <<< $line ## ignore helpfiles
		then
			if grep -q -v "test" <<< $line ## ignore test files
			then
				parse_pd_file $line
				((COUNT++))
			fi
		fi
	fi
	# if [[ $COUNT == 3 ]]
	# then
	# 	exit
	# fi
done < list.txt
echo "finished."
echo "parsed $COUNT abstractions"
exit