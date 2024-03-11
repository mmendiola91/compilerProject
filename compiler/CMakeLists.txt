cmake_minimum_required(VERSION 3.27)
project(CompilerMAIN)

set(CMAKE_CXX_STANDARD 17)

add_executable(CompilerMAIN main.cpp
        LexAnalyzer.cpp
        LexAnalyzer.h
        SyntaxAnalyzer.cpp
        SyntaxAnalyzer.h)
