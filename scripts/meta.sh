#!/bin/bash
#
#     update fd_lib .meta files
#
#
source scripts/absmap.sh || source absmap.sh

echo "updating list.txt..."
sh dirlist # update list.txt

echo "parsing files..."
COUNT=0
while read line; do
	if grep -q "fd_" <<<$line; then          ## only accept 'fd_'-type files
		if grep -q -v "-help.pd" <<<$line; then ## ignore helpfiles
			if grep -q -v "test" <<<$line; then    ## ignore test files
				parse_pd_file $line
				((COUNT++))
			fi
		fi
	fi
	# if [[ $COUNT == 3 ]]
	# then
	#     exit
	# fi
done <list.txt
echo "finished."
echo "parsed $COUNT abstractions"
exit

