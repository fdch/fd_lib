#!/bin/bash
# Makefile generator for fd_lib
NOW=$(date)
AUTHOR="Author: Fede Camara Halac"
AUTHEMAIL="Email: fch226@nyu.edu"
if [ ! $1 ]
then
PDLIBBUILDER_DIR=pd-lib-builder
else
PDLIBBUILDER_DIR="$1"
fi
FOUND=0
CLASSSOURCES=""
READMEFILES=""
HELPMEFILES=""
if [ ! -d $PDLIBBUILDER_DIR ]
then
echo "
================================================================================
$(tput setaf 1)error$(tput sgr 0):
================================================================================
Couldn't find Pd Lib Builder in: 
	$(tput setaf 1)$PDLIBBUILDER_DIR$(tput sgr 0)
	
Please provide a path to pdlibbuilder via argument, as in:
	$ /bin/bash fdlib path/to/pd-lib-builder

If you don't have pdlibbuilder, follow these instructions:
	$ git clone https://github.com/pure-data/pd-lib-builder pd-lib-builder
	$ /bin/bash fdlib pd-lib-builder
"
exit
else
CLASSSOURCES=$(find . -name "*.c" ! -name "*-*.c" | awk -v RS="" '{gsub (/\n/," ")}1')
HELPFILES=$(find . -name "*-help.pd" | awk -v RS="" '{gsub (/\n/," ")}1')
READMEFILES=$(find . -name "README.txt" -name "README.md"| awk -v RS="" '{gsub (/\n/," ")}1')
for i in $CLASSSOURCES, $HELPFILES, $READMEFILES
do 
((FOUND++))
done

echo "
#===============================================================================
# Makefile generated with fdlib.sh for pd lib builder
# $NOW
# $AUTHOR
# $AUTHEMAIL
#===============================================================================

lib.name = fd_lib

class.sources = $CLASSSOURCES

datafiles = $HELPFILES $READMEFILES

include $PDLIBBUILDER_DIR/Makefile.pdlibbuilder

#===============================================================================
" > Makefile
echo "$(tput setaf 2)
================================================================================
Thanks to the amazing Pd Lib Builder, you can simply do the following:
================================================================================
$(tput sgr 0)
To compile the library run:

	$ make

To build the entire library in <fd_lib>

	$ make install objectsdir=./

You can also compile single files, e.g., 

	$ make counter.pd_darwin
$(tput setaf 2)
================================================================================
$NOW
$FOUND .c, .pd and .md/.txt files
$AUTHOR
$AUTHEMAIL
$(tput sgr 0)"
fi
exit