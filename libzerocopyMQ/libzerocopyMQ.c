#include <stdio.h>
#include "comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <string.h>
#define TAM 1024

int createMQ(const char *cola) {
    int s;
    char *op;
    if((s = create_socket())<0){
        perror("error creando el socket");
        return -1;
    }

    op = "0";
    if (send_cabecera(s,op,(char *)cola)<0){
        perror("Error en el envio del codigo");
        return -1;
    }
	return 0;

}

int destroyMQ(const char *cola){
    return 0;
}
int put(const char *cola, const void *mensaje, uint32_t tam) {
    return 0;
}
int get(const char *cola, void **mensaje, uint32_t *tam, bool blocking) {
    return 0;
}
