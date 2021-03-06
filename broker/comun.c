/*
 * Incluya en este fichero todas las implementaciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */
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

struct mensaje_cola{
	int size;
	char * mensaje;
};


/***************************************************************************************************************
    Funcion que envia el mensaje al broker,  envia 2 parametros: El tamaño del mensaje y el mensaje 
    Recibe 2 parametros:
    - s: El identificador del socket
    - msg: El mensaje que se quiere enviar
    - tam: Tamaño del mensaje
    Devuelve 0 en caso de que no haya errores y -1 en caso de que los haya
****************************************************************************************************************/


int send_message(int s, char *msg,uint32_t tam){
    struct iovec iov[2];
    //Tamaño del mensaje
    iov[0].iov_base = &tam;
    iov[0].iov_len = sizeof(tam);
    //Mensaje
    iov[1].iov_base = msg; 
    iov[1].iov_len = tam;
    if(writev(s,iov,2)<0){
        return -1;
    }
    return 0;
}


//Funcion que devuelve al cliente 0 si la operacion ha sido correcta y -1 si ha sido incrorecta.  
int send_response(int s,int code){
	char *response;
	if(code==0){
		response = "0";
	}else{
		response = "1";
	}
	if(send(s,response,strlen(response)+1,0)<0){
		return -1;
	}
	return 0;
}

/**************************************************************************************************************
  Funcion que recibe la respuesta del broker de que la operacion se ha realizado con exito
  
  Devuelve 0 en caso de acieto y -1 en caso de fallo
 ****************************************************************************************************************/

int recv_response(int s){

    char *code;
    code = (char *)malloc(2);
    if (recv(s,code,2,0)<0){
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
