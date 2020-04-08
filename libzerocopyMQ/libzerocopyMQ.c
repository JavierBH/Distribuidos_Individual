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

/**************************************************************************************************************
 * Funcion que crea el socket ara la conexion
 * DEvuelve el identificador del socket en caso de que no haya erroes, y -1 en caso de que los haya
 ***************************************************************************************************************/
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

    //Se obtiene el host del broker
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

    // Se obtiene el puerto del broker
    port = getenv("BROKER_PORT");
    if(port == NULL){
        perror("El puerto del host es erróneo\n");
        return -1;
    }

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

/***************************************************************************************************************
    Funcion que envia la cabecera de la operacion,  envia 3 parametros: El cdigo de la operacion, el tamaño del
         nombre de la cola y el nombre de la cola 
    Recibe 3 parametros:
    - s: El identificador del socket
    - op: Codigo de operacion que equivale con las funciones: 
            0 -> createMQ
            1 -> destroyMQ
            2 -> put
            3 -> get
    - name_cola_ NOmbre de la cola

    Devuelve 0 en caso de que no haya errores y -1 en caso de que los haya
****************************************************************************************************************/

int send_cabecera(int s, char *op, char *name_cola){
    struct iovec iov[3];
    int size;
    size = sizeof(name_cola);
    //Codigo de operacion
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op);
    //Tamaño del nombre de la cola
    iov[1].iov_base = &size;
    iov[1].iov_len = sizeof(size);
    //Nombre de la cola
    iov[2].iov_base = name_cola; 
    iov[2].iov_len = strlen(name_cola);
    if(writev(s,iov,3)<0){
          perror("Error en el envio de la cabecera");
    	  return -1;
    }
    return 0;
}

int send_put(int s, char *op, char *name_cola,char *mensaje,int tam){
    struct iovec iov[5];
    int size;
    size = sizeof(name_cola);
    //Codigo de operacion
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op);
    //Tamaño del nombre de la cola
    iov[1].iov_base = &size;
    iov[1].iov_len = sizeof(size);
    //Nombre de la cola
    iov[2].iov_base = name_cola; 
    iov[2].iov_len = strlen(name_cola);
    //Tamaño del mensaje
    iov[3].iov_base = &tam;
    iov[3].iov_len = sizeof(tam);
    //Mensaje
    iov[4].iov_base = mensaje; 
    iov[4].iov_len = sizeof(mensaje);
    if(writev(s,iov,5)<0){
          perror("Error en el envio de la cabecera");
    	  return -1;
    }
    return 0;
}

int send_tam(int s, uint32_t tam){
    struct iovec iov[1];
    iov[0].iov_base=&tam;
    iov[0].iov_len=sizeof(tam);
    if(writev(s,iov,1)<0){
        perror("Error en el envio del mensaje");
        return -1;
    }
    return 0;
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
    int s;
    char *op;
    if((s = create_socket())<0){
        perror("error creando el socket");
        return -1;
    }

    op = "2";
    if (send_put(s,op,(char *)cola,(char *)mensaje,tam)<0){
        perror("Error en el envio del codigo");
        return -1;
    }
    
    return recv_response(s);
}

int get(const char *cola, void **mensaje, uint32_t *tam, bool blocking) {
    int s;
    char *op;
    if((s = create_socket())<0){
        perror("error creando el socket");
        return -1;
    }

    op = "3";
    if (send_cabecera(s,op,(char *)cola)<0){
        perror("Error en el envio del codigo");
        return -1;
    }

    if(recv_response(s)<0){
        perror("Error en la llegada de la respuesta");
        return -1;
    }
    
    if(send_tam(s,*tam)<0){
        perror("Error en el envio del tamaño");
        return -1;
    }

    if(recv_response(s)<0){
        perror("Error en la llegada de la respuesta");
        return -1;
    }

    if((*mensaje=recv_message(s))<0){
        perror("Error en la llegada de la respuesta");
        send_response(s,-1);
        return -1;
    }

    if(send_response(s,0)<0){
        perror("Error en el envio de la respuesta");
        return -1;
    }

    return recv_response(s);
}
