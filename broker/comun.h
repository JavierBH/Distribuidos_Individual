/*
 * Incluya en este fichero todas las definiciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */

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
int send_cod_op(int s, char *buff);
/*
    Funcion que envia la longitud del nombre de la cola y el nombre de la misma.
    HAce dos envios, el primero es la longitud de la cola, el segundo el nombre. 
    Recibe el socket y el nombre de la cola.
    Retorna 0 si es correcto y -1 si hay algun error
*/
int send_cola(int s, char *name_cola);

int send_cabecera(int s, char *op, char *name_cola);