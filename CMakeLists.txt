cmake_minimum_required(VERSION 3.10)
project(CVM_Plus_Plus VERSION 1.0)

# Force modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the src directory so headers can be found easily
include_directories(src)

# Main runtime executable
add_executable(cvm
    src/main.cpp
    src/frontend/lexer.cpp
    src/frontend/parser.cpp
    src/backend/compiler.cpp
    src/backend/vm.cpp
)

# Lightweight CLI visualizer for the language pipeline
add_executable(cvm_visualiser
    cvm/visualiser/main.cpp
    src/frontend/lexer.cpp
    src/frontend/parser.cpp
    src/backend/compiler.cpp
    src/backend/vm.cpp
)
