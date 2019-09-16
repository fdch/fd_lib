IOLETS=( "inlet" "outlet" "inlet~" "outlet~" )


function fetch_obj()
{
	awk '/^#X obj/' $1 | sed 's/;//g' | cut -f5 -d' ' | sort | uniq
}

function root_canvas()
{
	# return array with 2 line numbers
	# corresponding to a .pd file's canvas/restore line numbers
	# to obtain only the root canvas without it's subpatches
	# 
	# pass a filename as argument 1
	# 
	# TODO: no way of knowing which delimiter point is which
	# i.e. if it's start or end of a subpatch... but it works for root.


	
	local f=$1 # file
	local lc=0 # line count
	local d=0  # depth
	local s=() # subpatch delimiters (canvas->restore pairs)
	local l=''
	while read l
	do
		if grep -c -q "canvas" <<< "$l"
		then
			((d++)) # found canvas, increment depth
			if [[ $d > 1 ]]
			then
				s=( ${s[@]} ${lc}) # append line number to array (canvas point)
			fi
		elif grep -c -q "restore" <<< "$l"
		then
			if [[ $d > 1 ]]
			then
				s=( ${s[@]} ${lc}) # (restore point)
			fi
		fi
		((lc++)) # increment internal line count of file
	done < $f

	# return root line numbers A B, where root canvas is {0...A, B...END}
	if [[ $d > 1 ]]
	then
		echo ${s[0]} $((s[${#s[@]}-1]+1))
	else
		echo 0
	fi
}

function root_obj()
{
	# return how many matches were found
	# of a pattern "$1" (i.e. pd object name)
	# from first $2 lines and from line $3 until end (i.e. only root pd canvas)
	# in file $4
	awk -v p="[0-9] $1" -v b="$2" -v e="$3" '
		($0~p && NR < b) || ($0~p && NR > e) {count++}
		END{print count}
	' $4
}

function print_found()
{
	printf "[%-8s]:found %d in %s\n" "$1" "$2" "$3"
}

function find_iolets()
{
	local f=$1
	local n=0
	local c=($(root_canvas $f))
	local r=(0 0 0 0)
	local i=0
	local cnt=0
	if [[ $c > 1 ]]
	then
		for i in "${IOLETS[@]}"
		do
			n=$(root_obj "$i" ${c[@]} $f)
			if [[ $n ]]
			then
				# print_found $i $n $f
				r[$cnt]=$n
			fi
			((cnt++))
		done
	else # file has no depth
		for i in "${IOLETS[@]}"
		do
			n=$(grep -c "$i" "$f")
			# print_found $i $n $f
			r[$cnt]=$n
			((cnt++))
		done
	fi
	echo ${r[@]}
}


function check_args()
{
	local arg=$1
	local new=0
	local f=$2
	if grep -q "\$${arg}" <<< "${f}"
	then
		echo $arg # found arguments
		exit
	else
		if [[ $arg > 1 ]]
		then
			new=$((arg-1))
			check_args "$new" "${f}" # go to next argument and recurse 
		else
			echo 0 # no arguments
			exit
		fi
	fi
}


function find_args()
{
	local f=$1
	awk '
		/\$[1-9]/ && /^#X obj/
	' $f
}

function find_deps()
{
	local f=$1
	local found
	found=$(awk '
			/^\#X declare/ && (/-lib/ || /-path/)  
		' $f                 | 
		sed 's/.*declare //' |
		sed 's/-lib //g'     |
		sed 's/;//')
	
	if [[ $found ]]
	then
		echo "${found[@]}"
	else
		exit
	fi
}

function deps_string()
{
	local dn=${#@}
	local ds=''
	local i=0
	if [[ $dn -ge 1 ]] # more than one dependencies
	then
		for i in ${@}
		do
			ds="${ds}${i}:" # append dependency with colon
		done
		ds="${ds%:}" # just remove trailing colon
		echo ${dn} ${ds}
	elif [[ $dn == 1 ]] # exactly one dependency
	then
		ds=$1
		echo 1 ${ds}
	else # no deps
		ds='0'
		echo 0 0
	fi
}

function fetch_objc()
{
	# account for expanding stars and tildes or the universe explodes
	local f=$1
	echo "$(awk '/^#X obj/' $f | sed 's/;//g' | sed 's/\*/\\\\*/' | sed 's/\~/\\\\~/' |cut -f5 -d' ' | sort | uniq -c | sort -nr | awk '{print $1 " " $2}')"
}



function find_obj()
{
	# returns: classes objects obj_array (instances,class)
	local f=$1
	local fnd=0
	local count=0
	local i=0
	local r=0
	local ob=0

	fnd=($(fetch_objc $f))

	if [[ $fnd ]]
	then
		count=0
		r=0
		for (( i=0 ; i<${#fnd[@]} ; i+=2 ))
		do
			ob=${fnd[$((i))]}
			r=$((r+$ob))
			((count++))
		done
		#### TODO: total object count is WRONG!
		echo $count $r ${fnd[@]}
	else
		exit
	fi
}



function get_class_names()
{
	local a=("${@}")
	local ob=0
	local cln=0 # class name
	local vcl=0 # vanilla classes
	local isv=0
	local vclc=0
	local clnc=0
	local i=0
	local pat
	# echo ${a[@]} >&2
	for (( i=0 ; i<${#a[@]} ; i+=2 ))
	do
		ob=${a[$((i+1))]} # ${orig//[xyz]/_}
		pat=$(echo $ob | sed 's/\\*//; s/\\~//;q')
		# echo $pat >&2
		if grep -q -c "$pat" /tmp/vanilla
		then
			vcl+="${ob}:" # append classes with colon
			((vclc++))
		else
			cln+="${ob}:" # append classes with colon
			((clnc++))
		fi	
	done
	cln="${cln%:}" # just remove trailing colon
	vcl="${vcl%:}" # just remove trailing colon

	printf "%d %d %s %s" ${clnc} $(echo ${vclc}||echo -1) ${cln} ${vcl}
}

function find_connections()
{
	# returns: number of connections found
	local f=$1
	awk '
		/^#X connect/ {count++} 
		END{print count}
	' $f 
}

function get_vanilla_objects()
{
	echo > /tmp/vnl
	fetch_obj "${HELPINTRO}" > /tmp/vnl # add help-intro.pd file objects
	
	fetch_obj "${ALLGUIS}" >> /tmp/vnl  # add x_all_guis.pd file objects
	
	for i in $(ls ${EXTRA})
	do
		if [[ -d $i ]]
		then # directories in 'extra' dir are pdfiles 
			basename $i .pd >> /tmp/vnl
		elif grep -q -v "-help.pd" <<< "$i"
		then # ignore helpfiles
			if grep -q ".pd" <<< "$i"
			then # some pdfiles are inside 'extra' dir
				basename $i .pd >> /tmp/vnl
			fi
		fi
	done
	
	cat /tmp/vnl | sort | uniq > ${VCFILE} # sort vanilla file to avoid dups
	
}

function get_all_pd_files()
{
	local path=$1
	for i in $path/*.pd
	do 
		if grep -q -v "-help.pd" <<< "$i" ## ignore helpfiles
		then
			if grep -q -v "test" <<< "$i" ## ignore test files
			then 
				echo $i
			fi
		fi
	done
}

function parse_pd_file()
{
	local f=''
	if [[ ! $1 ]]
	then
		f=${1:-$(</dev/stdin)}
	else
		f="$1"
	fi

	if [[ ! -f $f ]]
	then
		echo "parse_pd_file(): exiting:"
		echo "File <$1> does not exist. Pass valid filename as argument."
		exit -1
	else
		printf "=%.0s" {1..20}
		printf " parsing [%s] ...\n" $f
	fi

	local fn=$(basename $f .pd)       # pd file name
	local fp=$(realpath $fn)             # path to file
	local m=${fp}/${fn}.meta             # path/to/file.meta (metadata)
	local args=$(find_args $f)        # string where arguments were found
	local ar=$(check_args 9 "${args}")   # arg count parsing 'args' string
	local io=($(find_iolets $f))      # array (in,ou,in~,ou~)
	local deps=($(find_deps $f))      # array of dependencies
	local ds=($(deps_string ${deps[@]})) # two values 'dep_num' 'deps'
	local connections=$(find_connections $f) # total number of connections
	local obj_array=($(find_obj $f))  # classes + total obj + obj data array
	local total_classes=${obj_array[0]}  # unique classes used (no repetitions)
	local total_objects=${obj_array[1]}  # total number of objects
	local classes=($(get_class_names ${obj_array[@]:2})) # get classes as str

	echo "fd_lib - metadata file - $(date)"                                > $m
	printf "patch_name   : %s\n" ${fn} 									  >> $m
	printf "patch_path   : %s\n" ${fp}                                    >> $m
	printf "inlet        : %d\n" ${io[0]}                                 >> $m
	printf "outlet       : %d\n" ${io[1]}                                 >> $m
	printf "inlet~       : %d\n" ${io[2]}                                 >> $m
	printf "outlet~      : %d\n" ${io[3]}                                 >> $m
	printf "arguments    : %d\n" ${ar}                                    >> $m
	printf "classes used : %d\n" ${total_classes}                         >> $m
	printf "total objects: %d\n" ${total_objects}                         >> $m
	printf "connections  : %d\n" ${connections}                           >> $m
	printf "dependencies : %d [%s]\n" ${ds[0]} ${ds[1]}                   >> $m
	printf "externals    : %d [%s]\n"    ${classes[0]} ${classes[2]}      >> $m
	printf "internals    : %d/%d [%s]\n" ${classes[1]} ${VC} ${classes[3]}>> $m

}

function batch_parse()
{
	get_all_pd_files $1 | parse_pd_file 
}
