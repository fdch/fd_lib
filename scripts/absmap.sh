#!/bin/bash

source scripts/_utils.sh || source _utils.sh

PDPATH="/Applications/Pd-0.50-0.app/Contents/Resources/"
EXTRA="$PDPATH/extra"
HELPINTRO="$PDPATH/doc/5.reference/help-intro.pd"
ALLGUIS="$PDPATH/doc/5.reference/x_all_guis.pd"
VC=0 # vanilla object total count
VCFILE=/tmp/vanilla

if [[ ! -d $PDPATH ]]
then
	echo "'$PDPATH' does not exist"
	echo "Please provide a path to the Pd directory"
	exit 1
else
	if [[ ! -d $EXTRA ]]
	then
		echo "'$EXTRA' does not exist"
		echo "Please make sure the 'extra' directory can be found."
		exit 2
	else
		if [[ ! -f $VCFILE ]]
		then
			get_vanilla_objects
		fi
		VC=$(awk 'END{print NR}' ${VCFILE})
	fi
fi
if [[ $2 != 0 ]] && [[ $1 ]]
then
	parse_pd_file $1
fi
# example usage
#
# parse_pd_file ./fd_log/fd_log.pd
# echo ./reflect/reflect.pd | parse_pd_file
# batch_parse /path/to/dir

