#!/bin/bash
#
# This script loads a pd patch (argument 1) and puts all dependencies in the 
# directory specified with argument 2
#

# get PD executable from aliases
shopt -s expand_aliases && source ~/.bash_alias

tmpsort=/tmp/sorted
tmplog=/tmp/log

function log_pdfile_load() {
  #load pd file and output console printout to logfile
  echo "Loading pd file..."
  $(pd -stderr -nogui -open "$1" 2> "$2") & 
  sleep 5
  kill $!
  wait 2> /dev/null
  echo "Done."
}

function parse_log() {
  grep 'succeeded' "$1" | 
  sort -u | 
  sed 's/tried //g' | 
  sed 's/and succeeded//g' | 
  sed 's/^.*: //' > $tmpsort 
}

function copy_files() {
  # copy all files into the lib directory
  while read line; do
    cp -r "$line" "$1/"
  done < $tmpsort
}

echo "**** Deps ****"

echo "Locating Pd..."

# check if pd exists and exit if it doesnt
if ! pd -version 2>&1; then 
  echo "pd not found";
  exit 1
fi

#get pdfile name from argument 1
if [ "$1" ]; then
	PDFILE="$1"
else
	echo "Please provide a pd patch in argument 1"
	exit 1
fi

#get libdir name from argument 2
if [ "$2" ]
then
	LIBDIR="$2"
else
	LIBDIR=$(echo "$PDFILE" | sed 's/.pd$//')
fi

echo "Packaging $PDFILE into $LIBDIR"

# load pd and get the console output into the temporary log file
log_pdfile_load "$PDFILE" $tmplog

echo "Parsing..."

# parse the log
parse_log $tmplog

# wait for use input
while true; do
  read -n 1 -p "The following files will be copied:
$(paste <(echo) $tmpsort)
Do you wish to continue? yY/nN" yn
  case $yn in
    [nN]* ) echo ; echo "Exiting." ; exit 0;;
    *) echo ; break;;
  esac
done

# make the lib directory
mkdir -p "$LIBDIR"
# copy the files
copy_files "$LIBDIR"
# show the dir structure
ls $LIBDIR/*

echo "**** Done. ****"
