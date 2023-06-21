cmake_minimum_required(VERSION 3.25)
project(battleship C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(battleship main.c src/board/board.c src/board/board.h src/point/point.c src/point/point.h src/ship/ship.h src/utils/console_colors.h src/server/session.c src/server/session.h src/client/client.c src/client/client.h)
