# INSTRUCTIONS

## Clone

	$ git clone https://github.com/fdch/fd_lib

## Compile

To use the objects, you'll need to compile them (only tested on macOS). For this you'll first need to initialize the [pd-lib-builder](https://github.com/pure-data/pd-lib-builder) submodule:

	$ cd fd_lib
	$ git submodule init (skip this step if you have pd-lib-builder already)

Then run these scripts:

	$ ./dirlist.sh
	$ ./fd_lib.sh [optional: /path/to/your/pd-lib-builder]

Read the output message. The usage is pretty much what pd-lib-builder suggests.

To compile all objects:

	$ make

To make individual objects:

	$ ./imake "sradio"

## Download

Alternatively, the zipfile [binaries.zip](binaries.zip) includes helpfiles and binaries (compiled for OSX). Abstractions are *not* included in the zip file.

## Declare

* Put a [declare] object on a Pd patch with the following arguments:

	[declare -path fd_lib]

* Alternatively: set the path from PREFERENCES>PATH dialog

## Deken

`fd_lib` is not yet on deken, but some of its abstractions have dependencies you can find on deken: *Help/Find Externals*:

* [Gem](https://gem.iem.at/)
* [ggee](https://github.com/pure-data/ggee)
