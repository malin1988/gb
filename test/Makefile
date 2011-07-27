gbtest: pkt.o gbtest.o
	gcc -o gbtest pkt.o gbtest.o
gbtest.o : gbtest.c
	gcc -c gbtest.c
pkt.o : pkt.c
	gcc -c pkt.c

.PHONY:clean
clean:
	rm *.o
