main : decode.o pkt.o main.o
	g++ -o main decode.o pkt.o main.o
main.o : main.cpp
	g++ -c main.cpp
decode.o : decode.cpp
	g++ -c decode.cpp
pkt.o : pkt.cpp
	g++ -c pkt.cpp

.PHONY:clean
clean:
	rm *.o
