# fd_lib
A suite of abstractions, objects, tutorials, and some shell scripts to work with Puredata.

## Get the library

If you have [pd-lib-builder](https://github.com/pure-data/pd-lib-builder), you can omit the *--recursive* flag:

	$ git clone --recursive https://github.com/fdch/fd_lib

## Compilation
Run the following and follow instructions on terminal:

	$ cd fd_lib
	$ ./fdlib.sh

If pd-lib-builder is somewhere else, you need to pass the path to pd-lib-builder as an argument:

	$ ./fdlib.sh /path/to/pd-lib-builder

## Binaries

Alternatively, the zipfile [binaries.zip](binaries.zip) includes helpfiles and binaries (compiled for OSX). Abstractions are *not* included in the zip file.


## Declare the library on Pd

* Put this object on a Pd patch

	[declare -path fd_lib]
	
* Alternatively, put this message and replace "/path/to/" with your actual path and click it:
	
	[;
	pd path-dialog 1 1 +/path/to/fd_lib, save_preferences(
	
* Alternatively: set the path from PREFERENCES>PATH dialog

## Brief overview

In Pd, right-click on any file to get the help file. For a full list of objects, see [list.txt](list.txt)

### Abstractions
Any file/folder that is preceded by "fd\_", e.g., [fd_norm](fd_norm)

### Objects
Any file/folder that is not preceded by "fd\_", e.g., [mainpath](mainpath)

### Tutorials

#### [pd-messages](tutorials/pd-messages)
A brief tutorial on (mostly) undocumented pd-related messages.

### Scripts

#### Building tools

* [dirlist.sh](dirlist.sh): tool to get helpfiles and abstractions into homonymous directories.
* [fdlib.sh](fdlib.sh): makes the Makefile required by pdlibbuilder

#### Workstation template
* [fdpd](fdpd): Sets up paired instances of Pd to work with Gem.

### Navigating through the library

####  [fd_navigator](fd_navigator)
Navigate throught the helpfiles using Cmd or Control + Left or Right

####  [fd_getfiles](fd_getfiles)
The help file for this abstraction includes a file navigation tool.

####  Open Pd's *Help Browser* (Cmd+b) or (ctrl+b) and navigate to the *fd_lib* entry

## Github pushes on branch <0.01>
Use and edit whatever you want, but please see the instructions below to push to the branch.

### Initialize git

	$ cd fd_lib
	$ git init

### Check what you have actually edited:

	$ git status

### Choose what you want to commit and add it like this

	$ git add <file1>
	$ git add <dir/file2>

### Alternatively, choose All Files:

	$ git add .

### Describe briefly:

	$ git commit -m "description"

### Double check:

	$ git remote add origin http://github.com/fdch/fd_lib
	$ git remote -v

### Push to the branch:

	$ git push -u origin 0.01

## Dependencies
Some abstractions depend on the following libraries. You can simply get them via deken (*Help/Find Externals*) or here:
* [ggee](https://github.com/pure-data/ggee)
* [iemguts](https://git.iem.at/pd/iemguts.git)
* [zexy](https://git.iem.at/pd/zexy.git)
* [Gem](https://gem.iem.at/)
* [cyclone](https://github.com/porres/pd-cyclone)
* [tof](https://github.com/electrickery/pd-tof)

## Issues
fch226 at nyu dot edu

