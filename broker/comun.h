/*
 * Incluya en este fichero todas las definiciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */
#include <stdio.h>
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
/*  Funcion que se utiliza para mandarle al broker el nombre de la cola que se quiere crear/acceder
    LOs codigos de operacion son los siguientes:
    0.- Crear cola
    1.- Destruir cola
    2.- Push mensaje
    3.- Pop Mensaje
*/

/*
    Funcion que manda el codigo de operacion. Recibe el socket y el codigo
    Retorna 0 si es correcto y -1 si hay algun error
*/

int send_message(int s, char *msg,uint32_t tam);
//Funcion que devuelve al cliente 0 si la operacion ha sido correcta y -1 si ha sido incrorecta.  
int send_response(int s,int code);
int recv_response(int s);