#!/bin/bash
PDLIBBUILDER_DIR="pd-lib-builder"
LIBNAME=fd_lib
COMMONLIB=src/fdLib.c
EXTFILE=doc/externals.pd
FDLIBVERSION=0.1
SOURCES=''
CSOURCES=0
for i in src/*.c
do
	if [[ "$i" != "$COMMONLIB" ]] && [[ "$i" != "src/$LIBNAME.c" ]]
	then
		SOURCES+="$i "
		((CSOURCES++))
	fi
done
DATAFILES=''
CABSTRAC=0
for i in abstractions/*.pd
do
	DATAFILES+="$i "
	((CABSTRAC++))
done
for i in help/*-help.pd *.txt
do
	DATAFILES+="$i "
done
if [[ ! -d $PDLIBBUILDER_DIR ]]
then
	if [[ "$1" ]] && [[ -d "$1" ]] && [[ -f "$1/Makefile.pdlibbuilder" ]]
	then
		PDLIBBUILDER_DIR="$1"
	else
		echo "Could not find '$PDLIBBUILDER_DIR'. Do you have pd-lib-builder?"
		echo "Pass the path to pd-lib-builder as argument 1 ..."
		echo "Or run the following:"
		echo "$ git submodule init"
		echo "$ git submodule update"
		exit 1
	fi
fi

echo "# Makefile for $LIBNAME version $FDLIBVERSION - `date`" > Makefile
printf "%s\n" "lib.name=$LIBNAME"		>> Makefile
echo "
# specify the location of main header file
cflags = -Iinclude
lib.setup.sources = src/\$(lib.name).c
" >> Makefile
printf "%s\n" "class.sources=$SOURCES" 	>> Makefile
echo "
common.sources=$COMMONLIB
" >> Makefile
printf "%s\n" "datafiles=$DATAFILES"	>> Makefile
echo "

define forLinux
  cflags += -std=c99
endef

# build a multi-object library
make-lib-executable=yes

# provide the path to pd-lib-builder
PDLIBBUILDER_DIR=$PDLIBBUILDER_DIR
include \$(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
" >> Makefile

X=10
Y=10
SX=80
SY=30
echo "#N canvas 500 50 530 480 12;
#X declare -lib fd_lib;
#X obj 363 10 declare -lib fd_lib;" > $EXTFILE
C=1
for i in $SOURCES;
do
	echo "#X obj $X $Y `basename $i .c`;" >> $EXTFILE
	Y=$((Y+$SY))
	modulo=$(($C%15))
	if [[ 0 == "$modulo" ]]
	then
		X=$((X+$SX))
		Y=10
	fi
	((C++))
done
echo "
$LIBNAME 

Current version: $FDLIBVERSION

This is my personal [Pure Data](https://github.com/pure-data/pure-data) library of abstractions and externals. 
It has around $CABSTRAC abstractions, $CSOURCES externals, shell scripts, and tutorials. 
For an overview of the library, go open '_overview.pd'
For instructions on how to compile see INSTALL.txt. 

$LIBNAME is now available via \`deken\`, Pure Data's package/externals manager. 
Open Pure Data and go to \`Help > Find Externals\`, then type \`$LIBNAME\` and download the binaries.

Use at your own risk and have fun!

fdch

" > README.txt
