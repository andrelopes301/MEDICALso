cmake_minimum_required(VERSION 3.20)
project(tp C)

set(CMAKE_C_STANDARD 99)

add_executable(tp medico.c medico.h cliente.c cliente.h balcao.c balcao.h medico.c)
