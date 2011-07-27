CXXFLAGS=-g -Wall
BINARIES=gb

all: tc.cpp utils.cpp
	g++ tc.cpp utils.cpp dister.cpp worker.cpp main.cpp -lTcApi -lpthread
main: tc.o dispatch.o ns.o ns_sdu.o
    g++ -o main tc.o dispatch.o ns.o ns_sdu.o
tc.o : tc.cpp utils.cpp
    g++ -c tc.cpp utils.cpp -lpthread -lTcApi
dispatch.o : worker.cpp dister.cpp utils.cpp
    g++ -c worker.cpp dister.cpp utils.cpp
ns.o : ns.cpp
    g++ -c ns.cpp
ns_sdu.o : ns_sdu.cpp
    g++ -c ns_sdu.cpp
