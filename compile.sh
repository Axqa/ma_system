#!/bin/bash

# Создаем директорию для сборки проекта
mkdir -p build
cd build

# Вызываем CMake для генерации Makefile
cmake ..

# Компилируем проект, используя Makefile
make
