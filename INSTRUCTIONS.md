# INSTRUCTIONS

## Clone

	$ git clone https://github.com/fdch/fd_lib

## Compile

To use the objects, you'll need to compile them, I have only tested on `macos`. For this you'll first need to initialize the [pd-lib-builder](https://github.com/pure-data/pd-lib-builder) submodule (skip this step if you have pd-lib-builder already):

	$ cd fd_lib
	$ git submodule init

Then run the following script to obtain the object list:

	$ sh dirlist

	$ sh fdlib [/path/to/your/pd-lib-builder]

Read the output message. The usage is pretty much what pd-lib-builder suggests.

To compile all object simply run:

	$ make

To make individual objects run `imake` and specify object name:

	$ sh imake "sradio"

To package `fd_lib` as a Pure Data library specify a target directory:

	$ sh make PDLIBDIR=/path/to/output/

## Declare

* Put a [declare] object on a Pd patch with the following arguments:

	[declare -path fd_lib]

* Alternatively: set the path from PREFERENCES>PATH dialog.

## Deken

`fd_lib` is now available on `deken`. Go to `Help > Find Externals` and type `fd_lib` or any of the object/abstraction names. 

## Dependencies

Find these on `deken`:
 
* [Gem](https://gem.iem.at/)
* [ggee](https://github.com/pure-data/ggee)
