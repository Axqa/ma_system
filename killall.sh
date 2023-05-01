#!/bin/bash

NAME="player"

if [ -n "$1" ]; then
  NAME="$1"
fi

# Получаем список идентификаторов процессов с заданным именем
# pids=$(pgrep "$1")
pids=$(pgrep "$NAME")

# Проверяем, что процессы были найдены
if [ -z "$pids" ]; then
  echo "Процессы с именем $NAME не найдены"
  exit 1
fi

# Останавливаем каждый процесс
for pid in $pids; do
  # echo "Остановка процесса $pid"
  kill "$pid"
done