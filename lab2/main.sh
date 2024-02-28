#!/bin/bash



#####CONSTANTS#####



PARAMETERS_COUNT=2

P_SORT_BY_NAME='-n'
P_SORT_BY_SIZE='-s'
P_SORT_BY_WIDTH='-w'
P_SORT_BY_CREATION_TIME='-t'
P_SORT_BY_MODIFICATION_TIME='-m'
P_SORT_BY_ACCESS_TIME='-a'
P_SORT_BY_RAND='-r'

ASCENDING_ORDER='asc'
DESCENDING_ORDER='desc'

LENGTH=4
PREFIX_PATTERN="^[0-9]\{$LENGTH\}\."



#####FUNCTIONS#####



function getFileNames {
	readarray -t fileNames < <(ls -p | grep -v /)
}


function fillFiles {
	local FILE_SIZE_INDEX=5
	local FILE_DATE_INDEX=6
	local FILE_TIME_INDEX=7

	local COMMAND='ls -l --time-style=long-iso'

	local sortingType="$1"

	fileSortingProperties=()

	local index
	for index in ${!fileNames[@]}
	do
		case $sortingType in
			$P_SORT_BY_NAME)
				fileSortingProperties+=("${fileNames[index]}")
			;;
			$P_SORT_BY_SIZE)
				fileSortingProperties+=($($COMMAND "${fileNames[index]}" | awk "{print \$$FILE_SIZE_INDEX }"))
			;;
			$P_SORT_BY_WIDTH)
				fileSortingProperties+=(${#fileName})
			;;
			$P_SORT_BY_CREATION_TIME)
				fileSortingProperties+=($($COMMAND --time=birth "${fileNames[index]}" | awk "{print \$$FILE_DATE_INDEX\$$FILE_TIME_INDEX }"))
			;;
			$P_SORT_BY_MODIFICATION_TIME)
				fileSortingProperties+=($($COMMAND --time=mtime "${fileNames[index]}" | awk "{print \$$FILE_DATE_INDEX\$$FILE_TIME_INDEX }"))
			;;
			$P_SORT_BY_ACCESS_TIME)
				fileSortingProperties+=($($COMMAND --time=atime "${fileNames[index]}" | awk "{print \$$FILE_DATE_INDEX\$$FILE_TIME_INDEX }"))
			;;
			$P_SORT_BY_RAND)
				fileSortingProperties+=($RANDOM)
			;;
		esac
	done
}


function gt {
	if [[ $1 =~ ^[0-9]+$ && $2 =~ ^[0-9]+$ ]]
	then
		if [ $1 -gt $2 ]
		then
			return 0
		else
			return 1
		fi
	else
		if [[ $1 > $2 ]]
		then
			return 0
		else
			return 1
		fi
	fi
}


function sortFiles {
	local sortStart=$1
	local sortEnd=$2

	if [ $((sortEnd - sortStart)) -le 1 ]
	then
		return
	fi

	local median=$sortStart
	local sortCurStart=$((median + 1))
	local sortCurEnd=$sortEnd
	local medianValue=${fileSortingProperties[$median]}
	local tmp
	
	while [ $sortCurStart -lt $sortCurEnd ]
	do
		tmp=${fileSortingProperties[$sortCurStart]}
		if gt $tmp $medianValue
		then
			sortCurEnd=$((sortCurEnd - 1))
			fileSortingProperties[$sortCurStart]=${fileSortingProperties[$sortCurEnd]}
			fileSortingProperties[$sortCurEnd]=$tmp
			tmp=${fileNames[$sortCurStart]}
			fileNames[$sortCurStart]=${fileNames[$sortCurEnd]}
			fileNames[$sortCurEnd]=$tmp
		else
			sortCurStart=$((sortCurStart + 1))
		fi
	done

	median=$((sortCurStart - 1))

	tmp=${fileSortingProperties[$median]}
	fileSortingProperties[$median]=${fileSortingProperties[$sortStart]}
	fileSortingProperties[$sortStart]=$tmp
	
	tmp=${fileNames[$median]}
	fileNames[$median]=${fileNames[$sortStart]}
	fileNames[$sortStart]=$tmp
	
	sortFiles $sortStart $median
	sortFiles $sortCurStart $sortEnd
}


function removePrefix {
	local index
	for index in ${!fileNames[@]}
	do
		local normalName=$(echo "${fileNames[index]}" | sed "s/$PREFIX_PATTERN//")	
		mv "${fileNames[index]}" "$normalName" &> /dev/null
	done
}


function assignPrefix {
	local order=$1
	
	local value
	local incrementor
	if [ "$order" = "$ASCENDING_ORDER" ]
	then
		value=1
		incrementor=1
	else
		value=${#fileNames[@]}
		incrementor=-1
	fi

	local index
	for index in ${!fileNames[@]}
	do
		local prefix=$(printf "%0${LENGTH}d" "$value")
		mv "${fileNames[index]}" "$prefix.${fileNames[index]}"
		value=$((value + incrementor))
	done
}



#####REST#####



getFileNames

if [ $# -eq 0 ]
then
	removePrefix
	exit
fi

if [ $# -ne $PARAMETERS_COUNT ]
then
	echo "Invalid number of arguments!" >&2
	exit
fi

case $2 in
	$ASCENDING_ORDER | $DESCENDING_ORDER)
		case $1 in
			$P_SORT_BY_NAME | $P_SORT_BY_SIZE | $P_SORT_BY_WIDTH | $P_SORT_BY_CREATION_TIME | $P_SORT_BY_MODIFICATION_TIME | $P_SORT_BY_ACCESS_TIME | $P_SORT_BY_RAND)
				removePrefix
				getFileNames
				fillFiles $1
				sortFiles 0 ${#fileNames[@]}
				assignPrefix $2
			;;
			*)
				echo "Invalid sorting argument!" >&2
			;;
		esac
	;;
	*)
		echo "Invalid order argument!" >&2
	;;
esac
