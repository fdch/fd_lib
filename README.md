# fd_lib
A suite of abstractions, objects, tutorials, and some shell scripts to work with Puredata.

## Get the library

	$ git clone http://github.com/fdch/fd_lib

## Compilation
Run this script and follow instructions. You might need to clone [pdlibbuilder](https://github.com/pure-data/pd-lib-builder) for this:

	$ ./fdlib.sh

## Declare the library on Pd
1. Put this object on a Pd patch

	[declare -path fd_lib]
	
1. Alternatively, put this message and replace "/path/to/" with your actual path and click it:
	
	[;
	pd path-dialog 1 1 +/path/to/fd_lib, save_preferences(
	
1. Alternatively: set the path from PREFERENCES>PATH dialog


## OVERVIEW

2. Use [fd_navigator.pd](fd_navigator/fd_navigator.pd) to navigate throught the helpfiles using Cmd or Control + Left or Right

3. See the help file for [fd_getfiles.pd](fd_getfiles/fd_getfiles.pd)

1. Open the HELP>Browser (Cmd+b) or (ctrl+b) and navigate to the *fd_lib* entry


## Github pushes on branch <0.01>
Use and edit whatever you want, but please see the instructions below to push to the branch.

Initialize git

	$ cd fd_lib
	$ git init

Check what you have actually edited:

	$ git status

Choose what you want to commit and add it like this

	$ git add <file1>
	$ git add <dir/file2>

Or choose All Files:

	$ git add .

Describe briefly:

	$ git commit -m "description"

Double check:

	$ git remote add origin http://github.com/fdch/fd_lib
	$ git remote -v

Push to the branch:

	$ git push -u origin 0.01

## Issues
fch226 at nyu dot edu