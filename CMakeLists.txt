cmake_minimum_required(VERSION 3.15)
project(DnDMonteCarloArena)

set(CMAKE_CXX_STANDARD 20)

add_executable(DnDMonteCarloArena main.cpp Source/Actor.h Source/Action.h Source/Group.h Source/Arena.cpp Source/Arena.h Source/Types.h Source/Group.cpp Source/Dice.cpp Source/Dice.h Source/Actor.cpp Source/Action.cpp Source/Output.cpp Source/Output.h)
