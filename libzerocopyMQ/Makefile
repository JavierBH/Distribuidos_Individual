CFLAGS=-Wall -g -fPIC

libzerocopyMQ.so: libzerocopyMQ.o comun.o
	$(CC) $(CFLAGS) -shared -o $@ $^

libzerocopyMQ.o: comun.h  zerocopyMQ.h

clean:
	rm -f *.o *.so


