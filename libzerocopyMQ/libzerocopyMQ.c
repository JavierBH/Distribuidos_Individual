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
#include <math.h>
#define TAM 1024

int check_file_length(uint32_t tam){
    if(tam >pow(2,32)+1 || tam<0){
        return -1;
    }
    return 0;
}


/**
 * Funcion que se encarga de crear el socket 
 * Devuelve el descriptor del socket en caso de acierto y -1 en caso de fallo
 * */

int create_socket(){
    int s, s_connect;
	struct sockaddr_in dir;
	struct hostent *host_info;
    char *host; // Host name
    char *port; // Host port
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		return -1;
	}

    //Se obtiene el host del broker
    host = getenv("BROKER_HOST");
    if(host == NULL){
        return -1;
    }

   
	host_info=gethostbyname(host);
	if(host_info == NULL){
        return -1;
    }

    // Se obtiene el puerto del broker
    port = getenv("BROKER_PORT");
    if(port == NULL){
        return -1;
    }

	dir.sin_addr=*(struct in_addr *)host_info->h_addr;
	dir.sin_port=htons(atoi(port));
	dir.sin_family=PF_INET;
	if ((s_connect = connect(s, (struct sockaddr *)&dir, sizeof(dir))) < 0) {
		close(s);
		return -1;
	}
  return s;
}

int check_name(char *cola){
    if(strlen(cola) > pow(2,16)+1){
        return -1;
    }
    return 0;
}

/**
 *   Funcion que envia la cabecera de la operacion, Recibe 4 argumentos:
 * - s: Descriptor del socket
 * - op: Codigo de operacion que equivale con las funciones: 
 *   0 -> createMQ
 *   1 -> destroyMQ
 *   2 -> put
 *   3 -> get
 * - name_cola_ NOmbre de la cola
 * - b: Identificador de get bloqueante
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/

int send_cabecera(int s, char *op, char *name_cola, int b){
    struct iovec iov[4];
    int size;
    char * blook;
    if(check_name(name_cola)<0){
        return -1;
    }
    if(b > 0){
        blook="1";
    }else{
        blook="0";
    }
    size = strlen(name_cola)+1;
    //Codigo de operacion
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op)+1;
    //Tamaño del nombre de la cola
    iov[1].iov_base = &size;
    iov[1].iov_len = sizeof(size);
    //Nombre de la cola
    iov[2].iov_base = name_cola; 
    iov[2].iov_len = strlen(name_cola)+1;
    //Get bloqueante
    iov[3].iov_base = blook;
    iov[3].iov_len = strlen(blook)+1;
    if(writev(s,iov,4)<0){
        close(s);
    	return -1;
    }
    return 0;
}

/**
 *   Funcion que envia la cabecera de la operacion, Recibe 4 argumentos:
 * - s: Descriptor del socket
 * - op: Codigo de operacion que equivale con las funciones: 
 *   0 -> createMQ
 *   1 -> destroyMQ
 *   2 -> put
 *   3 -> get
 * - name_cola_ NOmbre de la cola
 * - b: Identificador de get bloqueante
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/

int send_put(int s, char *op, char *name_cola,char *mensaje,uint32_t tam,int b){
    struct iovec iov[6];
    int size;
    char * blook;
    
    if(check_name(name_cola)<0){
        close(s);
        return -1;
    }
    if(check_file_length(tam)<0){
        close(s);
        return -1;
    }
    if(b > 0){
        blook="1";
    }else{
        blook="0";
    }
    size = strlen(name_cola)+1;
    //Codigo de operacion
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op)+1;
    //Tamaño del nombre de la cola
    iov[1].iov_base = &size;
    iov[1].iov_len = sizeof(size);
    //Nombre de la cola
    iov[2].iov_base = name_cola; 
    iov[2].iov_len = strlen(name_cola)+1;
    //Get bloqueante
    iov[3].iov_base = blook;
    iov[3].iov_len = strlen(blook)+1;
    //Tamaño del mensaje
    iov[4].iov_base = &tam;
    iov[4].iov_len = sizeof(tam);
    //Mensaje
    iov[5].iov_base = mensaje; 
    iov[5].iov_len = tam;
    if(writev(s,iov,6)<0){
        close(s);
    	return -1;
    }
    return 0;
}

/**
 *   Funcion que recibe el tamaño del mensaje, Recibe 1 argumentos:
 * - s: Descriptor del socket
 *  Devuelve el tamaño del mensaje en caso de que no haya errores y -1 en caso de fallo
 **/

int recv_tam(int s){
    int tam;
	tam = malloc(sizeof(uint32_t));
		//Se recibe el tamaño del mensaje
		if(read(s,&tam,sizeof(uint32_t))<0){
            close(s);
			return -1;
		}
    return tam;
}
/**
 *   Funcion que recibe el mensaje, Recibe w argumentos:
 * - s: Descriptor del socket
 * - tam: tamaño del mensaje a recibir
 * Devuelve el mensaje en caso de que no haya errores y NULL en caso de fallo
 **/

char * recv_message(int s,int tam){
	char *msg;
    msg = (char*)malloc(tam);
	//Se recibe el codigo de operacion
		if(recv(s,msg,tam,MSG_WAITALL)<0){
            close(s);
			return NULL;
		}

	return msg;
}

/**
 *   Funcion encargada de crear una cola, Recibe 1 argumento:
 * - cola: Nombre de la cola que se quiere crear
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/
int createMQ(const char *cola) {
    int s,res;
    char *op;
    
    if((s = create_socket())<0){
        return -1;
    }
     
    op = "0";
    if (send_cabecera(s,op,(char *)cola,0)<0){
        close(s);
        return -1;
    }
    res = recv_response(s);
    close(s);
    return res;
}
/**
 *   Funcion encargada de destruir una cola, Recibe 1 argumento:
 * - cola: Nombre de la cola que se quiere crear
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/
int destroyMQ(const char *cola){
    int s,res;
    char *op;
    if((s = create_socket())<0){
        return -1;
    }

    op = "1";
    if (send_cabecera(s,op,(char *)cola,0)<0){
        close(s);
        return -1;
    }
    
    res = recv_response(s);
    close(s);
    return res;
}
/**
 *   Funcion encargada de introducir un mensaje en una cola, Recibe 3 argumento:
 * - cola: Nombre de la cola que se quiere crear
 * - mensaje: mensaje que se quiere guardar
 * - tam: tamaño del mensaje
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/

int put(const char *cola, const void *mensaje, uint32_t tam) {
    int s, res;
    char *op;
    if((s = create_socket())<0){
        return -1;
    }
    op = "2";
    if (send_put(s,op,(char *)cola,(char *)mensaje,tam,0)<0){
        close(s);
        return -1;
    }

    res = recv_response(s);
    close(s);
    return res;
}
/**
 *   Funcion encargada de conseguir un mensaje de una cola, Recibe 4 argumentos:
 * - cola: Nombre de la cola de la  que se quiere obtener el mensaje
 * - mensaje: puntero donde se quiere almacenar el mensaje 
 * - tam: puntero donde se quiere almacenar el tamaño del mensaje
 * - blocking: Parametro que indica si el get es bloqueante
 * Devuelve 0 en caso de que no haya errores y -1 en caso de fallo
 **/

int get(const char *cola, void **mensaje, uint32_t *tam, bool blocking) {
    int s;
    char *op;
    if((s = create_socket())<0){
        return -1;
    }

    op = "3";
    if (send_cabecera(s,op,(char *)cola,blocking)<0){
        close(s);
        return -1;
    }
    if((*tam = recv_tam(s))<0){
        close(s);
        return -1;
    }
    if((*mensaje=recv_message(s,*tam))==NULL){
        close(s);
        return -1;
    }
    
    if(strncmp(*mensaje,"ERROR",5)==0){
        close(s);
        return -1;
    }

    if(strncmp(*mensaje,"0",1)==0){
        *tam=0;
        *mensaje="";
        return 0;
    }
    close(s);
    return 0;
}
