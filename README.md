Memoria de la practica

Broker

Para la implementacion del broker se ha decidido utilizar el modelo secuencial, a continuacion se van a exlicar los metodos utilizados en a implementacion del mismo:

- libera_cola(void *v): Funcion que libera el espacio de la cola

- libera_cola_dic(char *c, void *v): Funcion que libera el espacio asignado al diccionario

-  crea_cola(struct diccionario *d, struct diccionario *d_get, char *name):

 Funcion que se encarga de crear la cola y añadirla al diccionario, recibe 3 argumentos: 
  * d: Diccionario en el que se almacenan los mensajes
  * d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
  * name: nombre de la cola que se quiere crear
  
  Devuelve 0 en caso de acierto y -1 en caso de fallo

- elimina_cola(struct diccionario *d, struct diccionario *d_get, char *name): 

  Funcion que se encarga de eliminar la cola indicada, recibe 3 argumentos: 
 * d: Diccionario en el que se almacenan los mensajes
 * d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * name: nombre de la cola que se quiere crear
  Devuelve 0 en caso de acierto y -1 en caso de fallo

-