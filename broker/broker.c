#include <stdio.h>
#include "comun.h"
#include "diccionario.h"
#include "cola.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define TAM 1024

   // Funcion que libera la estructura de datos de la cola

void libera_cola(void *v){
	free(v);
}

void libera_cola_dic(char *c, void *v){
    free(c);
	struct cola *cola = v;
	if(cola_destroy(cola,libera_cola)<0){
		fprintf(stderr, "Error al eliminar la cola\n");
	}
}

 /*Esta funcion se ejecuta en el caso de que se quiera crear una cola
    Añade la cola al diccionario, asignandole un valor de la cola, ya que se encuentra vacia. DE la siguiente manera:
    Name_cola : struct cola*/

int crea_cola(struct diccionario *d, char *name){
	struct cola *c;
	if((c = cola_create())==NULL){
		perror("Error creando la cola");
		return -1;
	}

 if (dic_put(d, name, c) < 0){
        fprintf(stderr, "Cola duplicada \n");
		return -1;
	}
	return 0;
}

/*Esta funcion se ejecuta cuando llega una operacion de eliminar cola.
Busca en el diccionario la cola que se quiere eliminar y si existe la borra, junto a sus mensajes.
En caso de que no exista da un error*/

int elimina_cola(struct diccionario *d, char *name){
	struct cola *c;
	int error;
	if((c=dic_get(d,name,&error))==NULL){
		fprintf(stderr, "Cola no existente\n");
		return -1;
	}

	if (dic_remove_entry(d,name, libera_cola_dic) < 0){
    	    fprintf(stderr, "Error al eliminar la cola\n");
			return -1;
	}
	return 0;
}

    //Esta funcion se ejectua cuando lelga la orden de añadir un mensaje a la cola corresponiente

int escritura_mensaje(struct diccionario *d,char *cola, void *mensaje){
    int error ;
    struct cola *c;
    c = dic_get(d,cola,&error);

    if(error<0){
        perror("No existe la cola solicitida \n");
		return -1;
    }

   if(cola_push_back(c,mensaje)<0){
       perror("Error al introducir el mensaje en la cola solicitada \n");
	   return -1;
   }
   return 0;
}

 //   Esta funcion se ejectua cuando llega la orden de leer un mensaje

char *lectura_mensaje(struct diccionario *d, char *cola){
	int error;
    struct cola *c;
	char *msg;
    c = dic_get(d,cola,&error);
	if(error<0){
        fprintf(stderr, "No existe la cola solicitida \n");
		return NULL;
    }
	msg = cola_pop_front(c,&error);

	return msg;
}

int recv_tam(int s){
	int tam;
	if(read(s,&tam,sizeof(int))<0){
			perror("Error en la llegada del codigo de operacion");
			close(s);
			return -1;
		}
	return tam;
}

int main(int argc, char *argv[]) {
	int s, s_conec;
	unsigned int tam_dir;
	struct sockaddr_in dir;
	struct diccionario *d;
	int opcion=1;

	if (argc!=2) {
		fprintf(stderr, "Uso: %s puerto\n", argv[0]);
		return -1;
	}
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return -1;
	}
		
	/* Para reutilizar puerto inmediatamente */
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion))<0){
                perror("error en setsockopt");
                return -1;
        }

	dir.sin_addr.s_addr=INADDR_ANY;
	dir.sin_port=htons(atoi(argv[1]));
	dir.sin_family=PF_INET;
	if (bind(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("error en bind");
		close(s);
		return -1;
	}
	if (listen(s, 5) < 0) {
		perror("error en listen");
		close(s);
		return -1;
	}

	if((d = dic_create())==NULL){
		perror("Error al crear el diccionario");
	}

	while (1) {
		tam_dir=sizeof(dir);
		
		if ((s_conec=accept(s, (struct sockaddr *)&dir, &tam_dir))<0){
			perror("error en accept");
			close(s);
			return -1;
		}
	//-------- AQUI ACABA EL CODIGO DEL SOCKET --------
		char *op;
		char *name_cola;
		int error, size_name;

		op = malloc(sizeof(2));
		//Se recibe el codigo de operacion
		if(read(s_conec,op,1)<0){
			perror("Error en la llegada del codigo de operacion");
			close(s);
			return -1;
		}
		//Se recibe el tamaño del nombre de la cola
		if(read(s_conec,&size_name,sizeof(int))<0){
			perror("Error en la llegada del tamaño de la cola");
			close(s);
			return -1;
		}
		//Se reserva tamaño para el nombre de la cola
		fprintf(stderr,"%d",size_name);
		name_cola=malloc(size_name);
		//Se recibe el nombre de la cola
		if(read(s_conec,name_cola,size_name)<0){
			perror("Error en la llegada del nombre de la cola");
			close(s);
			return -1;
		}

		//Nota: op tiene el valor en ASCII
		switch (*op){
		case '0': //Crear Cola
			free(op);
			error = crea_cola(d,name_cola);	
			send_response(s_conec,error);
			break;
		case '1': //Destruir Cola
			free(op);
			error = elimina_cola(d,name_cola);
			free(name_cola);	
			send_response(s_conec,error);
			break;
		case '2': //put
			free(op);
			char *mensaje;
			if((mensaje = recv_message(s_conec))==NULL){
				send_response(s_conec,-1);
				perror("Error recibiendo el mensaje");
				break;
			}
			error = escritura_mensaje(d,name_cola,mensaje);	
			// Se envia la respuesta del mensaje
			send_response(s_conec,error);
			break;
		case '3': //get
			free(op);
			char *msg;
			int size;
			send_response(s_conec,0);
			if((size=recv_tam(s_conec))<0){
				send_response(s_conec,-1);
				perror("Error en el envio de la respuesta");
				break;
			}
			send_response(s_conec,0);
			msg = malloc(size);
			if((msg = lectura_mensaje(d,name_cola))==NULL){
				perror("Error en la lectura del mensaje");
				send_message(s_conec,"",sizeof(msg));
				send_response(s_conec,-1);
				break;
			}

			send_message(s_conec,msg,sizeof(msg));
			
			if(recv_response(s_conec)<0){
     		   perror("Error en la llegada de la respuesta");
        	   send_response(s_conec,-1);
			   break;
    		}

			send_response(s_conec,0);
			free(msg);
			break;
		default:
			perror("Error en el codigo de operacion");
			free(op);
			close(s_conec);
			break;
		}
		close(s_conec);
	}
	close(s);
	return 0;
}
