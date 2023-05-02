#!/bin/bash

HOST="localhost"
PORT="6000"
VERSION="Release"
BINARY="player"
TEAM_NAME="TeamName"
MODE="1"
INPUT="0"
AGENT_NUM="-1"
LOG_FILE=""

while getopts "h:p:v:b:t:a:l:" flag; do
	case "$flag" in
	h) HOST=$OPTARG ;;
	p) PORT=$OPTARG ;;
	v) VERSION=$OPTARG ;;
	b) BINARY=$OPTARG ;;
	t) TEAM_NAME=$OPTARG ;;
	a) AGENT_NUM=$OPTARG ;;
	l) LOG_FILE=$OPTARG ;;
	esac
done

CLIENT="./build/$BINARY"
LOG_DIR="Logfiles"
# mkdir $LOG_DIR 2>/dev/null
SLEEP_TIME=0.1

N_PARAM="-teamname $TEAM_NAME -host $HOST -port $PORT -input $INPUT"

A_PARAM="$N_PARAM"

if [ "$LOG_FILE" != "" ]; then
	A_PARAM="$A_PARAM -log $LOG_FILE -input 1"
fi

N_PARAM="$N_PARAM -mode $MODE"

i=1
while [ $i -le 11 ]; do
	# echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Player: $i"
	if [ "$AGENT_NUM" != "$i" ]; then
		echo $CLIENT $N_PARAM -num $i
		$CLIENT $N_PARAM -num $i &
	fi

	
	# sleep $SLEEP_TIME
	i=$(expr $i + 1)
done

if [ "$AGENT_NUM" != "-1" ]; then
	# Запускаем tmux для создания окна, в котором будем вводить строки
	tmux new-session -d -s my_session "./build/player $A_PARAM -num $AGENT_NUM"


	# Подключаемся к tmux окну и вводим строки
	tmux attach-session -t my_session
fi

sleep 3

wait

# pids=$(pgrep "$BINARY")

# # Останавливаем каждый процесс
# for pid in $pids; do
#   # echo "Остановка процесса $pid"
#   kill "$pid"
# done