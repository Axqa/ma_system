#!/bin/bash

x_start=-50
x_end=0
y_start=-30
y_end=30
step=10
size_start=3
size_end=5
size_step=1
# Создание файлов
for ((s = size_start; s <= size_end; s += size_step)); do
  for ((i = x_start; i <= x_end; i += step)); do
    for ((j = y_start; j <= y_end; j += step)); do
      filename="./configs/conf$(((i - x_start) / step * (y_end - y_start + step) / step + (j - y_start) / step + (s-size_start) * ((x_end - x_start + step) / step * (y_end - y_start + step) / step))).conf"

      echo "history_size $s" > "$filename"
      echo "mapper 2" >> "$filename"
      echo "predict_type 1" >> "$filename"
      echo "observer_pos ($i, $j)" >> "$filename"
    done
  done  
done