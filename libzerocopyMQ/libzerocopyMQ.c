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
	int s, s_connect, leido;
	struct sockaddr_in dir;
	struct hostent *host_info;
	char buf[TAM];
    char *host; // Host name
    char *port; // Host port
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return -1;
	}
    host = getenv("BROKER_HOST");
	host_info=gethostbyname(host);
	if(host_info == NULL){
        perror("La dirección IP del host es errónea\n");
        return -1;
    }

    port = getenv("BROKER_PORT");
    if(port == NULL){
        perror("El puerto del host es erróneo\n");
        return -1;
    }

	// 2 alternativas
	//memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dir.sin_addr=*(struct in_addr *)host_info->h_addr;
	dir.sin_port=htons(atoi(port));
	dir.sin_family=PF_INET;
	if (s_connect = connect(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error en connect");
		close(s);
		return -1;
	}
    char *buff;
    struct iovec iov[1];
     buff = "1";
     iov[0].iov_base = buff; 
     iov[0].iov_len = strlen(buff);
  /* writev(socket,iov structure, number of buffers refer in the iov structure) */
    if(writev(s_connect,iov,1)<0){
        perror("error en send");
		close(s);
		return -1;
    }
    close(s_connect);
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
