#!/bin/bash
#
# package the library
# instead of using make install from pdlibbuilder
# because it does not put the sources in it
#
#
target_dir="fd_lib"
if [[ $1 ]]; then
	target_dir="$1"
fi
mkdir -p "$target_dir/sources/pd-lib-builder"
cp fd_lib.* {.,abstractions,help}/*.pd "$target_dir"
cp -r examples doc data img scripts test "$target_dir"
cp Makefile *.{sh,txt} "$target_dir/sources/"
cp src/*.{h,c} "$target_dir/sources/"
cp pd-lib-builder/Makefile.pdlibbuilder "$target_dir/sources/pd-lib-builder/"
