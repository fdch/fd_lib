# OVERVIEW

A full list of `.pd` files shipped with the library is in [list.txt](list.txt)

## Navigating

Once you add ``fd_lib`` to Pure Data paths, the usual way would be to open the ``Help Browser`` and go to the ``fd_lib`` entry. All objects and abstractions have their corresponding help file which can be accessed by right-clicking on any object box. Abstractions are preceded by ``fd_`` (e.g., [fd_norm](fd_norm))

## Scripts

Some shell scripts are provided with the library:

* [deps](deps): This script loads a pd patch (argument 1) and puts all dependencies in the directory specified with argument 2.
* [fdpd](fdpd): Sets up paired instances of Pd to work with Gem.
* [dirlist.sh](dirlist.sh): tool to get helpfiles and abstractions into homonymous directories.
* [fdlib.sh](fdlib.sh): creates the Makefile required by pdlibbuilder.
* [imake](imake): compiles an individual object.
* [overview](overview): returns a Pd patch with all abstractions.
* [extern-overview](extern-overview): returns a Pd patch with all externals.
* [trim](trim): Trim a text between delimiters.
* [nohelpfiles.sh](nohelpfiles.sh): reports missing helpfiles.

## Tutorials

A brief tutorial on (mostly) undocumented pd-related messages can be found in [pd-messages](tutorials/pd-messages)
