all: tc.cpp utils.cpp
	g++ tc.cpp utils.cpp -lTcApi -lpthread
