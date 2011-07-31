CXXFLAGS = -Wall -std=c++0x

main : decode.o pkt.o main.o tc.o utils.o
	g++ -o main decode.o pkt.o main.o tc.o utils.o $(CXXFLAGS) -lTcApi -lpthread
main.o : main.cpp
	g++ -c main.cpp $(CXXFLAGS)
decode.o : decode.cpp
	g++ -c decode.cpp $(CXXFLAGS)
pkt.o : pkt.cpp
	g++ -c pkt.cpp $(CXXFLAGS)
tc.o : tc.cpp utils.cpp
	g++ -c tc.cpp $(CXXFLAGS) -lTcApi -lpthread
utils.o : utils.cpp
	g++ -c utils.cpp $(CXXFLAGS)

.PHONY:clean
clean:
	rm *.o
