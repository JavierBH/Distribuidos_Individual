#include <stdio.h>
#include "comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include<netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
int createMQ(const char *cola) {
    int s;
	struct sockaddr_in dir;
	struct hostent *host_info;
    char *host; // Host name
    char *port; // Host port

	if ((s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return 1;
	}
    host = getenv("BROKER_HOST");
	host_info=gethostbyname(host);
	if(host_info == NULL){
        perror("La dirección IP del host es errónea\n");
        return 1;
    }

    port = getenv("BROKER_PORT");
    if(port == NULL){
        perror("El puerto del host es erróneo\n");
        return 1;
    }

    dir.sin_addr = *((struct in_addr *)host_info->h_addr);	dir.sin_port=htons(atoi(port));
	dir.sin_family=AF_INET;
	if (connect(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error en connect");
		close(s);
		return 1;
	}

    // AQUI VA EL CODIGO QUE HACE LO DE LA FUNCION

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
