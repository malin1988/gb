CXXFLAGS=-g -Wall
BINARIES=gb

all: tc.cpp utils.cpp
	g++ tc.cpp utils.cpp dister.cpp worker.cpp main.cpp -lTcApi -lpthread
