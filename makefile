CXX = g++
CXXFLAGS = -g -Wall -std=c++0x

INC=-I/usr/include -I../common -I./
LIB=-lsfml-graphics -lsfml-system -lsfml-window -lGL -lGLU


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@ $(INC)

all: prog

prog: main.o myvectors.o
	$(CXX) $(CXXFLAGS) -o prog $^ $(LIB)

myvectors.o:
	$(CXX) $(CXXFLAGS) -c ../common/myvectors.cpp $^ $(LIB) -o $@