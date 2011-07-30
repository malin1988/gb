main : decode.o pkt.o main.o tc.o utils.o
	g++ -o main decode.o pkt.o main.o tc.o utils.o -lTcApi -lpthread
main.o : main.cpp
	g++ -c main.cpp
decode.o : decode.cpp
	g++ -c decode.cpp
pkt.o : pkt.cpp
	g++ -c pkt.cpp
tc.o : tc.cpp utils.cpp
	g++ -c tc.cpp -lTcApi -lpthread
utils.o : utils.cpp
	g++ -c utils.cpp

.PHONY:clean
clean:
	rm *.o
