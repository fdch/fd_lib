#!/bin/bash
FEDE="Enjoy and if you have any issues, please contact me at <fdch.github.io>"
USAGE="<pdstart> is a utility to create a working tree for Pure Data projects.

With it you can start a Pure Data project using a directory tree. 
The idea is for you to work from only one place (your <bin>)
and make very fast backups with one single command. The program takes 
one mandatory argument indicating the project <dirname>,
and an optional filename for the root main file (defaults to "main"). 
The program outpus a directory tree and opens Pure Data with 
the root patch <file> (aka. "main"). From then on, you just go to 
your <bin> and start working.

Usage:
	$ pdstart <dirname> [<filename>]

$FEDE

"
#------------------------------------------------------------------------------
#
#	DIRECTORY TREE:
#
#	The program outputs a default file structure like this:
#
#	name				<-- the project name provided in argument 1
#	│
# 	├── backup.sh 		<-- the backup utility
# 	├── bak 			<-- a directory for backups (see backup utility below)
# 	│   ├── bin_1 		<-- backups with increasing indices...
# 	│   │    ├── lib
# 	│   │    └── main.pd
# 	│	└── bin_2 		<-- backups with increasing indices...
# 	│	└── ...	 		<-- backups with increasing indices...
# 	├── bin 			<-- the main working directory
# 	│   ├── backup -> ../backup.sh <-- the symbolic link to backup utility
# 	│   ├── lib 		<--	the dir with all pd files called by <file>.pd
# 	│   └── main.pd 	<-- the main <file> (default name: "main")
# 	├── rec 			<-- to keep audio recordings (excluded from backup)
# 	└── txt 			<-- to keep text files (excluded from backup)
# 	└── ... 			<-- all other directories that are added to $DIRS
#
#------------------------------------------------------------------------------
#
#	BACKUP UTILITY:
#
BAKHELP="
This backup utility is created by <pdstart> to ease the creation of backups
as your project gets bigger and bigger.	This backup utility resides 
in the root folder (aka. <name>) with links created on each working <bin>. 

Usage:
	$ cd bin
	$ sh backup

All your backups are created in the <bak> directory	with increasing indeces.

$FEDE
"
#------------------------------------------------------------------------------
#
#	Below are some handy variables you can edit
#
#------------------------------------------------------------------------------

#	Pure Data directory name (app version) to run pd afterwards
WHICHPD=Pd-0.49-1.app

#	Main canvas Width and Height
W=450
H=300
#	Main canvas position on screen
SCREENX=530
SCREENY=50
#	Main canvas Font size
F=14

#	Default root patch name
DEFFILE=main.pd

#	<bin> and <lib> directories
BINDIR=bin # Your working directory
LIBDIR=lib # All your abstractions/externals can go here
BAKDIR=bak # Your backups (see Backup Utility)

#------------------------------------------------------------------------------
#
#	Default directories to create:
#
#	These will not be included in the backups that the backup utility does,
#	because you might not want duplicates of the same files
#	that don't change every time you want to backup.
#
#	You can append/change as many as you like.	
#
#------------------------------------------------------------------------------
DIRS=( rec txt )




#	careful when editting beyond this point...




#------------------------------------------------------------------------------
#
#	Pure Data template file 
#	Create a file with a [declare] object inside a subpatch (for tidiness)
#
#------------------------------------------------------------------------------
PATHS="-path ./$LIBDIR"

for p in ${DIRS[@]} ; do
	PATHS+=" " # just add a space
	PATHS+="-path ../$p"
done

PDTEMPLATE="\
#N canvas $SCREENX $SCREENY $W $H $F;
#X declare $PATHS;
#N canvas 0 20 450 300 declare 0;
#X obj 20 20 declare $PATHS;
#X restore 20 20 pd declare;
"
DACTEMPLATE="\
#N canvas 1060 52 142 208 vol 1;
#X obj 39 26 vsl 15 128 0 127 0 0 vol vol-set vol 0 -9 0 10 -262144
-1 -1 0 1;
#X obj 40 161 nbx 5 14 -1e+37 1e+37 0 0 empty vol empty 0 -8 0 10 -262144
-1 -1 0 256;
#X obj 64 31 vu 15 120 vu vu -1 -8 0 10 -66577 -1 1 0;
#X restore 23 58 pd vol;
#N canvas 0 22 450 300 dac 0;
#X obj 39 23 catch~ mono;
#X obj 39 49 clone -s 1 fd_dacout 2 vol;
#X obj 39 75 env~;
#X obj 39 101 - 100;
#X obj 39 127 s vu;
#X connect 0 0 1 0;
#X connect 1 0 2 0;
#X connect 2 0 3 0;
#X connect 3 0 4 0;
#X restore 22 88 pd dac;
"
#------------------------------------------------------------------------------
#
#	This is a backup command utility for easier (non-destructive) backup
#
#	Usage:
#			$ cd bin
#			$ sh backup
#
#------------------------------------------------------------------------------

#	Backup command name
BAKLINK=backup
#	The actual command (the name will be just a link)
BAKCMD=$BAKLINK.sh

BACKUP="\
#!/bin/sh
#-----------------------------------------------------------------------------
#
#	<$BAKCMD> - A Pure Data backup utility
#
#	Automatically generated from <pdstart>
#
#	Usage: from <$BINDIR> directory holding <$FILE>, run 
#
#	\$ sh $BAKCMD
#
#-----------------------------------------------------------------------------

#	Directory holding backups
BAKDIR=$BAKDIR

#	<$BINDIR> directory holding all you need to backup
BINDIR=$BINDIR

#	Help
BAKHELP=\"$BAKHELP\"

if [[ \$1 ]]; then
	echo \"\$BAKHELP\"
	exit
fi

#	Find next backup item nubmer
i=1

if [[ -d ../\$BAKDIR/\${BINDIR}_1 ]] ; then
	for dirs in ../\$BAKDIR/* ; do 
		((i++))
	done
fi

BAKFILE=\${BINDIR}_\${i}

echo \"Backing up \$BINDIR into \$BAKDIR/\$BAKFILE ...\"

rsync -aP --exclude=$BAKLINK ../\$BINDIR/* ../\$BAKDIR/\$BAKFILE

exit"
#------------------------------------------------------------------------------
#
#	Stuff to run Pure Data
#
#------------------------------------------------------------------------------

#	pd path
PDPATH=/Applications
#	pd command
PDBIN=Contents/Resources/bin/pd
#	pd command flags
PDFLAGS="-open"

#	A function to open a Pure Data file
function run_pd() {
	echo "opening: $FILE ..."
	$PDPATH/$WHICHPD/$PDBIN $PDFLAGS $FILE
}
#------------------------------------------------------------------------------
#
#	A function to make default directories
#
#------------------------------------------------------------------------------
function make_dirs() {
	#	Make the main bin and lib
	if [[ ! -d $BINDIR ]]; then
		echo "creating $BINDIR ..."
		mkdir $BINDIR
		mkdir $BINDIR/$LIBDIR
	else
		#	bin exists, make only lib
		echo "$BINDIR already exists..."
		if [[ ! -d $BINDIR/$LIBDIR ]]; then
			mkdir $BINDIR/$LIBDIR
		else
			#	Dont make bin or lib
			echo "$BINDIR/$LIBDIR already exist..."
		fi
	fi
	#	Make the Bakup directory if it doesn't exist
	if [[ ! -d ./$BAKDIR ]]; then
		echo "creating $BAKDIR ..."
		mkdir ./$BAKDIR
	fi
	#	Make all other directories if they don't exist
	for i in ${DIRS[*]}; do
		if [[ ! -d ./$i ]]; then
			echo "creating $i ..."
			mkdir ./$i
		fi
	done
}
#------------------------------------------------------------------------------
#
#	The main function
#
#------------------------------------------------------------------------------
function main() {
	make_dirs
	echo "cd ./$BINDIR"
	#	Make the backup command
	echo "creating $BAKCMD in $BINDIR ..."
	echo "$BACKUP" > $BAKCMD
	#	Move to bin dir
	cd ./$BINDIR
	#	Create a link to the backup command
	ln -s "../$BAKCMD" "$BAKLINK" 
	#	Make the Pure Data file
	echo "creating $FILE in $BINDIR ..."
	echo "$PDTEMPLATE" > $FILE 
	echo "#X text 200 20 \"$NAME\";" >> $FILE
	echo "$DACTEMPLATE" >> $FILE 
}
#------------------------------------------------------------------------------
#
#	Run this script
#
#	Only create project root dir if it doesn't exist.
#	Only open $FILE if it exists (either in . or in ./bin)
#
#	If it doesn't exist, then 
#	create all dirs and initialize the working tree
#
#------------------------------------------------------------------------------
if [[ $1 ]]; then 
	NAME=$1
	if [[ -d $NAME ]]; then
		echo "<$NAME> already exists. Exiting."
		exit
	else
		#	Check for filename in argument 2 or fall back to $DEFFILE
		if [[ $2 ]]; then 
			FILE=`basename $1 .pd`.pd
		else 
			FILE=$DEFFILE
		fi
		mkdir $NAME
		cd $NAME
		if [[ -f $FILE ]]; then
			echo "$FILE already exists."
		else 
			if [[ -f $BINDIR/$FILE ]]; then 
				echo "$BINDIR/$FILE already exists."
				cd $BINDIR
			else
				main
			fi
		fi
	fi
else 
	echo "$USAGE"
	exit
fi
#------------------------------------------------------------------------------
#
#	Run Pure Data (with <time> ... just out of curiosity)
#
#------------------------------------------------------------------------------
time run_pd
exit