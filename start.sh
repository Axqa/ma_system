#!/bin/bash

HOST="localhost"
PORT="6000"
VERSION="Release"
BINARY="player"
TEAM_NAME="TeamName"
MODE="1"
INPUT="0"

while getopts "h:p:v:b:t:" flag; do
	case "$flag" in
	h) HOST=$OPTARG ;;
	p) PORT=$OPTARG ;;
	v) VERSION=$OPTARG ;;
	b) BINARY=$OPTARG ;;
	t) TEAM_NAME=$OPTARG ;;
	esac
done

CLIENT="./build/$BINARY"
LOG_DIR="Logfiles"
# mkdir $LOG_DIR 2>/dev/null
SLEEP_TIME=0.1

N_PARAM="-teamname $TEAM_NAME -host $HOST -port $PORT -mode $MODE -input $INPUT"

i=1
while [ $i -le 11 ]; do
	echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Player: $i"
	echo $CLIENT $N_PARAM -num $i
	$CLIENT $N_PARAM -num $i &
	sleep $SLEEP_TIME
	i=$(expr $i + 1)
done
sleep 3

wait

# pids=$(pgrep "$BINARY")

# # Останавливаем каждый процесс
# for pid in $pids; do
#   # echo "Остановка процесса $pid"
#   kill "$pid"
# done