#!/bin/bash
LIBNAME=fd_lib
FDLIBVERSION=0.3.1
DESCRIPTION="A suite of externals and abstractions"
AUTHOR="Fede Camara Halac"
LICENSE="GPL -2+"
COMMONLIB=src/fdLib.c
EXTFILE=doc/externals.pd
PDLIBBUILDER_DIR="pd-lib-builder"
R=README.txt
META=fd_lib-meta.pd
DATADIRS="doc data img include pd-lib-builder scripts src test"
# -----------------------------------------------------------------------------
#
# Load all sources
#
# -----------------------------------------------------------------------------
SOURCES=''
CSOURCES=0
for i in src/*.c; do
  if [[ "$i" != "$COMMONLIB" ]] && [[ "$i" != "src/$LIBNAME.c" ]]; then
    SOURCES+="$i "
    ((CSOURCES++))
  fi
done
# -----------------------------------------------------------------------------
#
# Load all datafiles (abstractions, help files, texts)
#
# -----------------------------------------------------------------------------
DATAFILES=''
# store number of abstractions here for README.txt
CABSTRAC=0
# load abstractions
for i in abstractions/*.pd; do
  DATAFILES+="${i} "
  ((CABSTRAC++))
done
# make sure readme and meta file are there
touch $R
touch $META
# continue loading help files and text files
for i in help/*.pd *.txt $META config.sh; do
  DATAFILES+="${i} "
done
# -----------------------------------------------------------------------------
#
# Check if pd-lib-dir exists here, otherwise make sure argument 1 has it
#
# -----------------------------------------------------------------------------
if [[ ! -d $PDLIBBUILDER_DIR ]]; then
  if [[ "$1" ]] && [[ -d "$1" ]] && [[ -f "$1/Makefile.pdlibbuilder" ]]; then
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
# -----------------------------------------------------------------------------
#
# Make Makefile
#
# -----------------------------------------------------------------------------
echo "# Makefile for $LIBNAME version $FDLIBVERSION" >Makefile
echo "lib.name=$LIBNAME" >>Makefile
# specify the location of main header file
echo "lib.setup.sources = src/\$(lib.name).c" >>Makefile
# all classes
echo "class.sources = $SOURCES" >>Makefile
# the common lib
echo "common.sources = $COMMONLIB" >>Makefile
# all abstractions, help files, and texts
echo "datafiles = $DATAFILES" >>Makefile
# all sub directories
echo "datadirs = $DATADIRS" >>Makefile
# C flags
echo "cflags= -Iinclude" >>Makefile
# standard c99 flag for linux
echo "define forLinux" >>Makefile
echo "    cflags += -std=c99" >>Makefile
echo "endef" >>Makefile
# build a multi-object library
echo "make-lib-executable=yes" >>Makefile
# provide the path to pd-lib-builder
echo "PDLIBBUILDER_DIR=$PDLIBBUILDER_DIR" >>Makefile
echo "include \$(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder" >>Makefile
# -----------------------------------------------------------------------------
#
# Make externals.pd
#
# -----------------------------------------------------------------------------
X=10
Y=10
SX=80
SY=30
echo "#N canvas 500 50 530 480 12;" >$EXTFILE
echo "#X declare -lib $LIBNAME;" >>$EXTFILE
echo "#X obj 360 $Y declare -lib $LIBNAME;" >>$EXTFILE
C=1
for i in $SOURCES; do
  echo "#X obj $X $Y $(basename $i .c);" >>$EXTFILE
  Y=$((Y + $SY))
  M=$(($C % 15))
  if [[ 0 == "$M" ]]; then
    X=$((X + $SX))
    Y=10
  fi
  ((C++))
done
# -----------------------------------------------------------------------------
#
# Make README.txt
#
# -----------------------------------------------------------------------------
echo "$LIBNAME" >$R
echo >>$R
echo "Current version: $FDLIBVERSION" >>$R
echo "This is my personal Pure Data library of abstractions and externals." >>$R
echo "Get Pure Data here: https://msp.ucsd.edu" >>$R
echo "It has:" >>$R
echo "    $CABSTRAC abstractions," >>$R
echo "    $CSOURCES externals," >>$R
echo "    shell scripts, and more." >>$R
echo "For an overview of the library, go open '_overview.pd'" >>$R
echo "For instructions on how to compile see INSTALL.txt." >>$R
echo >>$R
echo "$LIBNAME is now available via \`deken\`, Pd's package manager." >>$R
echo "Open Pure Data and go to \`Help > Find Externals\`," >>$R
echo "then type \`$LIBNAME\` and download the binaries." >>$R
echo >>$R
echo "Use at your own risk and have fun!" >>$R
echo "fdch" >>$R
# -----------------------------------------------------------------------------
#
# Make fd_lib-meta.pd
#
# -----------------------------------------------------------------------------
echo "#N canvas 1 51 320 90 10;" >$META
echo "#N canvas 1 51 380 134 META 1;" >>$META
echo "#X text 10 10 NAME $LIBNAME;" >>$META
echo "#X text 10 30 VERSION $FDLIBVERSION;" >>$META
echo "#X text 10 40 DESCRIPTION $DESCRIPTION;" >>$META
echo "#X text 10 50 AUTHOR $AUTHOR;" >>$META
echo "#X text 10 90 LICENCE $LICENSE;" >>$META
echo "#X restore 43 36 pd META;" >>$META
echo "#X text 39 15 $LIBNAME: $DESCRIPTION;" >>$META
# -----------------------------------------------------------------------------
#
# Update version on header
#
# -----------------------------------------------------------------------------
sed -i "s/\(#define FDLIBVERSION\).*$/\1 \"$FDLIBVERSION\";/" ./include/fdLib.h
