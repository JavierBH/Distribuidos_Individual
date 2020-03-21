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

int create_socket(){
  int s, s_connect;
	struct sockaddr_in dir;
	struct hostent *host_info;
  char *host; // Host name
  char *port; // Host port
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return -1;
	}
    host = getenv("BROKER_HOST");
    if(host == NULL){
        perror("Error en la variable del HOST\n");
        return -1;
    }
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
	if ((s_connect = connect(s, (struct sockaddr *)&dir, sizeof(dir))) < 0) {
		perror("error en connect");
		close(s);
		return -1;
	}
  return s;
}

int send_cabecera(int s, char *op, char *name_cola){
    struct iovec iov[2];
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op);
    iov[1].iov_base = name_cola; 
    iov[1].iov_len = strlen(name_cola);
    if(writev(s,iov,2)<0){
		  return -1;
    }
    return 0;
}

int recv_response(int s){

    char *code;
    code = (char *)malloc(2);
    if (recv(s,code,sizeof(code),0)<0){
        perror("Error en la respuesta");
        return -1;
    } 

    if(strcmp(code,"0")==0){
        free(code);
        return 0;
    } else {
        free(code);
        return -1;
    }
}

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
    return recv_response(s);
}

int destroyMQ(const char *cola){
    int s;
    char *op;
    if((s = create_socket())<0){
        perror("error creando el socket");
        return -1;
    }

    op = "1";
    if (send_cabecera(s,op,(char *)cola)<0){
        perror("Error en el envio del codigo");
        return -1;
    }
    
    return recv_response(s);
}

int put(const char *cola, const void *mensaje, uint32_t tam) {
    return 0;
}

int get(const char *cola, void **mensaje, uint32_t *tam, bool blocking) {
    return 0;
}
