# Basic make file.

all: smoke

matrices: main.cpp matrices.cpp matrices.hpp
	clang++ -g -Wall -Wextra -std=c++17 -o matrices main.cpp matrices.cpp

smoke: matrices
	cd smoke_test && ./smoke_test.sh ../matrices
