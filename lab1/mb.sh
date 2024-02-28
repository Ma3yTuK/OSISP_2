#!/bin/bash


###names###


blankTile=0
crossTile=1
plusTile=2

singleShip=0
doubleShip=1
tripleShip=2
quadrupleShip=3

redColor=$'\033[0;31m'
noColor=$'\033[0m'

fieldWidth=10
fieldHeight=10

tileWidth=8
tileHeight=4

currentRow=0
currentCol=0

player1ID=0
player2ID=1

player=0

screenShotFile=~/screen.txt

horizontalOrientation=0
verticalOrieantation=1


###values###


declare -A playerShips
declare -A playerShipsRows
declare -A playerShipsCols
declare -A playerShipsOrientation
declare -A playerShipsHits

playerShipsSize[$player1ID]=0
playerShipsSize[$player2ID]=0

ships=($singleShip $doubleShip $tripleShip $quadrupleShip)
players=($player1ID $player2ID)

declare -A shipsFields;
declare -A userFields;

maxShipCount[$singleShip]=4
maxShipCount[$doubleShip]=3
maxShipCount[$tripleShip]=2
maxShipCount[$quadrupleShip]=1

playerScores[$player1ID]=0
playerScores[$player2ID]=0

playerNames[$player1ID]="Player1"
playerNames[$player2ID]="Player2"

shipCount[$singleShip]=0
shipCount[$doubleShip]=0
shipCount[$tripleShip]=0
shipCount[$quadrupleShip]=0

shipSizes[$singleShip]=1
shipSizes[$doubleShip]=2
shipSizes[$tripleShip]=3
shipSizes[$quadrupleShip]=4

shipTiles[$singleShip]=$crossTile
shipTiles[$doubleShip]=$crossTile
shipTiles[$tripleShip]=$crossTile
shipTiles[$quadrupleShip]=$crossTile

tiles[$blankTile]=$(cat << EOF
 ------ 
|      |
|      |
 ------ 
EOF
)
tiles[$crossTile]=$(cat << EOF
 ------ 
|xxxxxx|
|xxxxxx|
 ------ 
EOF
)
tiles[$plusTile]=$(cat << EOF
 ------ 
|######|
|######|
 ------ 
EOF
)


###functions###


function toLeft {
	if (( $currentCol != 0 ))
	then
		currentCol=$((currentCol - 1))
		return 0
	fi
	return 1
}

function toRight {
	if (( $currentCol != $((fieldWidth - 1)) ))
	then
		currentCol=$((currentCol + 1))
		return 0
	fi
	return 1
}

function toTop {
	if (( $currentRow != 0 ))
	then
		currentRow=$((currentRow - 1))
		return 0
	fi
	return 1
}

function toBottom {
	if (( $currentRow != $((fieldHeight - 1)) ))
	then
		currentRow=$((currentRow + 1))
		return 0
	fi
	return 1
}

function createPlayerShip {
	local ship=$1
	local orientation=$2
	local player=${players[$player]}
	playerShip=${playerShipsSize[$player]}
	playerShips[$player,$playerShip]=$ship
	playerShipsRows[$player,$playerShip]=$currentRow
	playerShipsCols[$player,$playerShip]=$currentCol
	playerShipsOrientation[$player,$playerShip]=$orientation
	playerShipsHits[$player,$playerShip]=0
	playerShipsSize[$player]=$(( $playerShip + 1 ))
}

function addHorizontally {
	local player=${players[$player]}
	local ship=$1
	local size=${shipSizes[$ship]}	
	if (( $((currentCol + size - 1)) >= $fieldWidth )) || (( ${shipCount[$ship]} >= ${maxShipCount[$ship]} ))
	then
		return 1
	fi
	local left=$(( currentCol > 0 ? currentCol - 1 : 0))
	local right=$(( currentCol + size < fieldWidth ? currentCol + size : fieldWidth - 1))
	local top=$(( currentRow > 0 ? currentRow - 1 : 0))
	local bottom=$((currentRow + 1 < fieldHeight ? currentRow + 1 : fieldHeight - 1))
	for i in $(seq $top $bottom)
	do
		for j in $(seq $left $right)
		do
			if ! [ -z ${shipsFields[$player,$i,$j]} ]
			then
				return 1
			fi
		done
	done
	createPlayerShip $ship $horizontalOrientation
	for i in $(seq 0 $((size - 1)))
	do
		local col=$((currentCol+i))
		userFields[$player,$currentRow,$col]=${shipTiles[$ship]}
		shipsFields[$player,$currentRow,$col]=$playerShip
	done
	shipCount[$ship]=$((${shipCount[$ship]} + 1))
}
	
function addVertically {
	local player=${players[$player]}
	local ship=$1
	local size=${shipSizes[$ship]}	
	if (( $((currentRow + size - 1)) >= $fieldHeight )) || (( ${shipCount[$ship]} >= ${maxShipCount[$ship]} ))
	then
		return 1
	fi
	local left=$(( currentCol > 0 ? currentCol - 1 : 0))
	local right=$(( currentCol + 1 < fieldWidth ? currentCol + 1 : fieldWidth - 1))
	local top=$(( currentRow > 0 ? currentRow - 1 : 0))
	local bottom=$((currentRow + size < fieldHeight ? currentRow + size : fieldHeight - 1))
	for i in $(seq $top $bottom)
	do
		for j in $(seq $left $right)
		do
			if ! [ -z ${shipsFields[$player,$i,$j]} ]
			then
				return 1
			fi
		done
	done
	createPlayerShip $ship $verticalOrientation
	for i in $(seq 0 $((size - 1)))
	do
		local row=$((currentRow+i))
		userFields[$player,$row,$currentCol]=${shipTiles[$ship]}
		shipsFields[$player,$row,$currentCol]=$playerShip
	done
	shipCount[$ship]=$((${shipCount[$ship]} + 1))
}

function resetShipsFields {
	shipsFields=""
}

function resetUserFields {
	userFields=""
	for i in $(seq 0 $((fieldHeight - 1)))
	do
		for j in $(seq 0 $((fieldWidth - 1)))
		do
			for playr in ${players[@]}
			do
				userFields[$playr,$i,$j]=$blankTile
			done
		done
	done
}

function resetShipCounts {
	for ship in ${ships[@]}
	do
		shipCount[$ship]=0
	done
}

function resetPlayerScores {
	for playr in ${players[@]}
	do
		playerScores[$playr]=0
	done
}

function resetPlayerShips {
	playerShips=""
	playerShipsRows=""
	playerShipsCols=""
	playerShipsOrientation=""
	playerShipsHits=""
	for playr in ${players[@]}
	do
		playerShipsSize[$playr]=0
	done
}

function isFilled {
	local player=${players[$player]}
	if (( ${playerShipsSize[$player]} == 10 ))
	then
		return 0
	else
		return 1
	fi
}

function changeTileColor {
	local resultField="$1"
	local row=$2
	local col=$3
	local color=$4
	local lineFrom=$((row*tileHeight+1))
	local lineTo=$(((row+1)*tileHeight))
	local from=".{$tileWidth}"
	local to="${color}&${noColor}"
	local linePos=$((col+1))
	echo "$resultField" | sed -r "$lineFrom,$lineTo s/$from/$to/$linePos"
}

function genField {
	local player=${players[$player]}
	for j in $(seq 0 $((fieldWidth - 1)))
	do
		local column="${tiles[${userFields[$player,0,$j]}]}"
		for i in $(seq 1 $((fieldHeight - 1)))
		do
			column+=$'\n'
			column+="${tiles[${userFields[$player,$i,$j]}]}"
		done
		local result=$(paste -d "" <(echo "$result") <(echo "$column"))
	done
	changeTileColor "$result" $currentRow $currentCol $redColor
}

function update {
	local field=$(genField)
	clear
	echo "$message"
	echo "$field"
}

function resetCurrent {
	currentRow=0
	currentCol=0
}

function getPlayerName {
	echo ${playerNames[${players[$player]}]}
}

function setupField {
	message="$(getPlayerName) place your ships"
	update
	while true
	do
		read -rsn1 input
		case $input in
			'z')
				if addHorizontally $singleShip
				then
					if isFilled 
					then	
						break 
					fi
					update
				fi
			;;
			'Z')
				if addVertically $singleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'x')
				if addHorizontally $doubleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'X')
				if addVertically $doubleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'c')
				if addHorizontally $tripleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'C')
				if addVertically $tripleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'v')
				if addHorizontally $quadrupleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'V')
				if addVertically $quadrupleShip
				then
					if isFilled 
					then 
						break 
					fi
					update
				fi
			;;
			'a')
				if toLeft 
				then 
					update 
				fi
			;;
			'd')
				if toRight 
				then 
					update 
				fi
			;;
			'w')
				if toTop 
				then 
					update 
				fi
			;;
			's')
				if toBottom 
				then 
					update 
				fi
			;;
		esac
	done
}

function switchPlayer {
	player=$(( ($player + 1) % ${#players[@]} ))
}

function couldHit {
	local player=${players[$player]}
	if (( ${userFields[$player,$currentRow,$currentCol]} == $blankTile ))
	then
		return 0
	else
		return 1
	fi
}

function gameFinished {
	local player=${players[$player]}
	if (( ${playerScores[$player]} == 20 ))
	then
		return 0
	else
		return 1
	fi
}

function hit {
	local player=${players[$player]}
	local playerShip=${shipsFields[$player,$currentRow,$currentCol]}
	if [ -z $playerShip ]
	then
		userFields[$player,$currentRow,$currentCol]=$plusTile
		return 1
	fi
	userFields[$player,$currentRow,$currentCol]=${shipTiles[${playerShips[$player,$playerShip]}]}
	playerScores[$player]=$(( ${playerScores[$player]} + 1 ))
	playerShipsHits[$player,$playerShip]=$(( ${playerShipsHits[$player,$playerShip]} + 1 ))
}

function killed {
	local player=${players[$player]}
	local playerShip=${shipsFields[$player,$currentRow,$currentCol]}
	local ship=${playerShips[$player,$playerShip]}
	local size=${shipSizes[$ship]}
	if (( ${playerShipsHits[$player,$playerShip]} != $size ))
	then
		return 1
	fi
	local row=${playerShipsRows[$player,$playerShip]}
	local col=${playerShipsCols[$player,$playerShip]}
	case ${playerShipsOrientation[$player,$playerShip]} in
		$horizontalOrientation)
			local left=$(( col > 0 ? col - 1 : 0))
			local right=$(( col + size < fieldWidth ? col + size : fieldWidth - 1))
			local top=$(( row > 0 ? row - 1 : 0))
			local bottom=$((row + 1 < fieldHeight ? row + 1 : fieldHeight - 1))
			if (( $left != $col ))
			then
				for i in $(seq $top $bottom)
				do
					userFields[$player,$i,$left]=$plusTile
				done
			fi
			if (( $right == $col + $size ))
			then
				for i in $(seq $top $bottom)
				do
					userFields[$player,$i,$right]=$plusTile
				done
			fi
			if (( $top != $row ))
			then
				for j in $(seq $left $right)
				do
					userFields[$player,$top,$j]=$plusTile
				done
			fi
			if (( $bottom == $row + 1 ))
			then
				for j in $(seq $left $right)
				do
					userFields[$player,$bottom,$j]=$plusTile
				done
			fi
		;;
		$verticalOrientation)
			local left=$(( col > 0 ? col - 1 : 0))
			local right=$(( col + 1 < fieldWidth ? col + 1 : fieldWidth - 1))
			local top=$(( row > 0 ? row - 1 : 0))
			local bottom=$((row + size < fieldHeight ? row + size : fieldHeight - 1))
			if (( $left != $col ))
			then
				for i in $(seq $top $bottom)
				do
					userFields[$player,$i,$left]=$plusTile
				done
			fi
			if (( $right == $col + 1 ))
			then
				for i in $(seq $top $bottom)
				do
					userFields[$player,$i,$right]=$plusTile
				done
			fi
			if (( $top != $row ))
			then
				for j in $(seq $left $right)
				do
					userFields[$player,$top,$j]=$plusTile
				done
			fi
			if (( $bottom == $row + $size ))
			then
				for j in $(seq $left $right)
				do
					userFields[$player,$bottom,$j]=$plusTile
				done
			fi
		;;
	esac
}

function play {
	message="$(getPlayerName) turn"
	update
	local currentField=$shipsField1ID
	while true
	do
		read -rsn1 input
		case $input in
			'a')
				if toLeft 
				then 
					update 
				fi
			;;
			'd')
				if toRight 
				then 
					update 
				fi
			;;
			'w')
				if toTop 
				then 
					update 
				fi
			;;
			's')
				if toBottom 
				then 
					update 
				fi
			;;
			'p')
				update > $screenShotFile
			;;
			$'\0A')
				if couldHit
				then
					if hit
					then
						if killed
						then
							if gameFinished
							then
								echo "$(getPlayerName) won"
								break
							fi
						fi
					else
						switchPlayer
						resetCurrent
						message="$(getPlayerName) turn"
					fi
					update
				fi
			;;
		esac
	done
}

function game {
	resetShipsFields
	resetUserFields
	resetPlayerShips
	resetShipCounts
	resetCurrent
	setupField
	resetShipCounts
	resetCurrent
	switchPlayer
	setupField
	resetUserFields
	resetCurrent
	resetPlayerScores
	switchPlayer
	play
}

###rest###
	
game
