cmake_minimum_required(VERSION 3.10)
project(Platonica)
include_directories(include)

# GLOB allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

add_executable(Platonica ${SOURCES})