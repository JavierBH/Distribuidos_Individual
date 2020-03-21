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
Funcion que crea el socket del cliente
Devuelve el identificador del socket si no hay ningun error y -1 si ha habido algun error.
*/


int recv_response(int s){
  char *op;
  op = malloc(8);
  
  if (recv(s,op,sizeof(op),MSG_DONTWAIT)<0){
    perror("Error en la respuesta");
    return -1;
  }
  if(strcmp(op,"OK")==0){
    free(op);
    return 0;
  } else{
    free(op);
    return -1;
  }
}