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

void send_op(int op,char* cola,void*mensaje);
void send_number(int op);
/*Method that starts the socket in the client */
void conect();