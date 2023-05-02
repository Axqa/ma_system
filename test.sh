#!/bin/bash


# Запускаем tmux для создания окна, в котором будем вводить строки
tmux new-session -d -s my_session "./build/player"


# Подключаемся к tmux окну и вводим строки
tmux attach-session -t my_session
