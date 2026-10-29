cmake_minimum_required(VERSION 4.0)
project(lab1)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab1 main.cpp
        Structures.h
        db.h
        db.cpp
        Array.h
        Array.cpp
        Slist.h
        Slist.cpp
        Dlist.h
        Dlist.cpp
        Stack.h
        Stack.cpp
        Queue.h
        Queue.cpp
        Tree.cpp
        Tree.h)
