#!/bin/bash
PDLIBBUILDER_DIR="./pd-lib-builder"
LIBNAME=fd_lib
COMMONLIB=src/fdLib.c
SOURCES=''
for i in src/*.c
do
	if [[ "$i" != "$COMMONLIB" ]] && [[ "$i" != "src/$LIBNAME.c" ]]
	then
		SOURCES+="$i "
	fi
done
DATAFILES=''
for i in help/*-help.pd abstractions/*.pd *.txt
do
	DATAFILES+="$i "
done
if [[ ! -d $PDLIBBUILDER_DIR ]]
then
	echo $PDLIBBUILDER_DIR
	echo "Define path to pd-lib-builder on line 2"
	exit 1
fi

echo "# Makefile for $LIBNAME - `date`" > Makefile
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
#X obj 363 10 declare -lib fd_lib;" > externals.pd
C=1
for i in $SOURCES;
do
	echo "#X obj $X $Y `basename $i .c`;" >> externals.pd
	Y=$((Y+$SY))
	modulo=$(($C%15))
	if [[ 0 == "$modulo" ]]
	then
		X=$((X+$SX))
		Y=10
	fi
	((C++))
done

