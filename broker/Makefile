CFLAGS=-Wall -g

all: broker demo_dic demo_cola

demo_dic: demo_dic.o diccionario.o 

demo_cola: demo_cola.o cola.o 

broker: broker.o comun.o diccionario.o cola.o

broker.o: comun.h

clean:
	rm -f *.o broker demo_dic demo_cola
