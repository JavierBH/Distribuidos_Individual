/*
 * Incluya en este fichero todas las implementaciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */
#include "comun.h"
#include <sys/uio.h>
#include <string.h>
/*
    Funcion que manda el codigo de operacion. Recibe el socket y el codigo
    Retorna 0 si es correcto y -1 si hay algun error
*/
int send_cod_op(int s, char *buff){
    struct iovec iov[1];
    iov[0].iov_base = buff; 
    iov[0].iov_len = strlen(buff);
  /* writev(socket,iov structure, number of buffers refer in the iov structure) */
    if(writev(s,iov,1)<0){
		return -1;
    }
    return 0;
}

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

int send_cola(int s, char *name_cola){
    char *size_cola;
    size_cola="8";
    struct iovec iov[2];
    iov[0].iov_base = size_cola; 
    iov[0].iov_len = strlen(size_cola);
    iov[1].iov_base = name_cola; 
    iov[1].iov_len = strlen(name_cola);

 //SI hay un error la funcion devuelve -1
    if(writev(s,iov,2)<0){
		  return -1;
    }
    return 0;
}