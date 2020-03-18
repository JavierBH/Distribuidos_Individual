/*
 * Incluya en este fichero todas las implementaciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */
#include "comun.h"
#include <sys/uio.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
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

/*
  Funcion que envia la operacion que se va a ejecutar y el nombre de la cola. 
  DEvuelve 0 si la operacion ha sido correcto *1 si ha sido incorrecta.
*/
int send_cabecera(int s, char *op, char *name_cola){
    struct iovec iov[2];
    iov[0].iov_base = op; 
    iov[0].iov_len = strlen(op);
    iov[1].iov_base = name_cola; 
    iov[1].iov_len = strlen(name_cola);
   // iov[2].iov_base = name_cola; 
   // iov[2].iov_len = strlen(name_cola);
    if(writev(s,iov,2)<0){
		  return -1;
    }
    return 0;
}
/*
Funcion que crea el socket del cliente
Devuelve el identificador del socket si no hay ningun error y -1 si ha habido algun error.
*/
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