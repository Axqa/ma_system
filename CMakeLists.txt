cmake_minimum_required(VERSION 3.5)

project(test5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_NAME player)

set(SOURCES
  src/parser.cpp
  src/parser.h
  src/logger.cpp
  src/logger.h
  src/connection.cpp
  src/connection.h
  src/utilities.cpp
  src/utilities.h
  src/objects.cpp
  src/objects.h
  src/enums.h
  src/worldmodel.cpp
  src/worldmodel.h
  src/acthandler.cpp
  src/acthandler.h
  src/messagehandler.cpp
  src/messagehandler.h
  src/player.cpp
  src/player.h
  src/formation.cpp
  src/formation.h
  src/soccertypes.cpp
  src/soccertypes.h
  src/worldmodelmapping.cpp
  )

set(OTHER
    formations.conf
    )


file(COPY ${OTHER} DESTINATION ".")
set(MAIN src/main.cpp)

add_executable(${PROJ_NAME} ${MAIN} ${SOURCES})