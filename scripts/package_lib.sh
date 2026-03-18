#!/bin/bash
#
# package the library
#
#
mkdir -p fd_lib/sources/pd-lib-builder
cp fd_lib.* {.,abstractions,help}/*.pd fd_lib/
cp -r examples doc data img scripts test fd_lib/
cp Makefile *.{sh,txt} fd_lib/sources/
cp src/*.{h,c} fd_lib/sources/
cp pd-lib-builder/Makefile.pdlibbuilder fd_lib/sources/pd-lib-builder/
