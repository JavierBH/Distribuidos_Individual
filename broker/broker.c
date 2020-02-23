#include <stdio.h>
#include "comun.h"
#include "diccionario.h"
#include "cola.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM 1024
struct mensajes{
    char * mensaje;
};
/*
    Funcion que libera la estructura de datos de la cola
 */
void libera_cola(void *c){
    free(c);
}
/* Esta funcion se ejecuta en el caso de que se quiera crear una cola
    Añade la cola al diccionario, asignandole un valor de la cola, ya que se encuentra vacia. DE la siguiente manera:
    Name_cola : struct cola
*/
void *crea_cola(struct diccionario *d, char *name){
 if (dic_put(d, name, cola_create()) < 0)
        fprintf(stderr, "Cola duplicada \n");
}

/*
Esta funcion se ejecuta cuando llega una operacion de eliminar cola.
Busca en el diccionario la cola que se quiere eliminar y si existe la borra, junto a sus mensajes.
En caso de que no exista da un error 
*/

void *elimina_cola(struct diccionario *d, char *name){
if (dic_remove_entry(d,name, libera_cola) < 0)
        fprintf(stderr, "Cola no existente\n");
}

void * servicio(void *arg){
	int s, leido;
	char buf[TAM];

	s=(long) arg;

	while ((leido=read(s, buf, TAM))>0) {
		if (write(s, buf, leido)<0) {
			perror("error en write");
			close(s);
			return NULL;
		}
	}
	if (leido<0) {
		perror("error en read");
		close(s);
		return NULL;
	}
	close(s);
	return NULL;
}


int main(int argc, char *argv[]) {
	int s, s_conec;
	unsigned int tam_dir;
	struct sockaddr_in dir, dir_cliente;
	int opcion=1;
	pthread_t thid;
	pthread_attr_t atrib_th;
    struct diccionario *d;
        if (argc!=2) {
                fprintf(stderr, "Uso: %s puerto\n", argv[0]);
                return 1;
        }

	/* Creamos los thread de tipo detached para que liberen sus
	recursos al terminar sin necesidad de que se haga un
	pthread_join */
	pthread_attr_init(&atrib_th);
	pthread_attr_setdetachstate(&atrib_th, PTHREAD_CREATE_DETACHED);

	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return 1;
	}

	/* Para reutilizar puerto inmediatamente */
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion))<0){
                perror("error en setsockopt");
                return 1;
        }

	dir.sin_addr.s_addr=INADDR_ANY;
	dir.sin_port=htons(atoi(argv[1]));
	dir.sin_family=PF_INET;
	if (bind(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error en bind");
		close(s);
		return 1;
	}

	if (listen(s, 5) < 0) {
		perror("error en listen");
		close(s);
		return 1;
	}
	while (1) {
		tam_dir=sizeof(dir_cliente);
		if ((s_conec=accept(s, (struct sockaddr *)&dir_cliente, &tam_dir))<0){
			perror("error en accept");
			close(s);
			return 1;
		}
		pthread_create(&thid, &atrib_th, servicio, (void *)(long)s_conec);

		/* Esta forma de pasar el parámetro no sería válida
		puesto que se produce una condición de carrera */
		//pthread_create(&thid, NULL, servicio, &s_conec);
	}

	close(s);
