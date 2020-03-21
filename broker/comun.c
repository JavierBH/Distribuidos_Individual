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
