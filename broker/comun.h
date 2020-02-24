/*
 * Incluya en este fichero todas las definiciones que pueden
 * necesitar compartir el broker y la biblioteca, si es que las hubiera.
 */

/*  TIpo opaco que se utiliza para mandarle al broker el nombre de la cola que se quiere crear/acceder
    LOs codigos de operacion son los siguientes:
    0.- Crear cola
    1.- Destruir cola
    2.- Push mensaje
    3.- Pop Mensaje
*/
struct nombre_cola;