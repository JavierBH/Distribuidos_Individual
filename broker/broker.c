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

// Estructura encargada de almacenar el mensaje y su tamaño
struct mensaje_cola{
	int size; // tamaño del mensaje
	char * mensaje; //mensaje
};

//Funcion que libera el espacio de la cola
void libera_cola(void *v){
	free(v);
}

//FUncion que libera el espacio asignado al diccionario
void libera_cola_dic(char *c, void *v){
 	struct cola *cola;
	cola = v;
	cola_destroy(cola,libera_cola);
	free(c);
	
}


/**
 * Funcion que se encarga de crear la cola y añadirla al diccionario, recibe 3 argumentos: 
 * - d: Diccionario en el que se almacenan los mensajes
 * - d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */

int crea_cola(struct diccionario *d, struct diccionario *d_get, char *name){
	struct cola *c,*c_g;
	if((c = cola_create())==NULL){
		return -1;
	}
	if((c_g = cola_create())==NULL){
		return -1;
	}
	//Se añaden las colas a los diccionarios
 	if(dic_put(d, name, c) < 0){
		return -1;
	}
 	if(dic_put(d_get, name, c_g) < 0){
		return -1;
	}
	return 0;
}

/**
 * Funcion que se encarga de eliminar la cola indicada, recibe 3 argumentos: 
 * - d: Diccionario en el que se almacenan los mensajes
 * - d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */

int elimina_cola(struct diccionario *d, struct diccionario *d_get, char *name){
	int *error;
    struct cola *c;
	int *s_conec;
	error = malloc(sizeof(int));
    c = dic_get(d_get,name,error);
	if(error<0){
		return -1;
    }
	//Se manda un mensaje a cada uno de los clientes que estaban esperando
	while((s_conec = cola_pop_front(c,error))!= NULL && error>0 ){
		send_response(*s_conec,1);
		close(*s_conec);
		free(s_conec);
	}
	free(error);

	//Se elimina la cola indicada
	if (dic_remove_entry(d,name, libera_cola_dic) < 0){
			return -1;
	}
	return 0;
}

/**
 * Funcion que se encarga de recivir un mensaje del cliente, recibe 1 argumento: 
 * - s: DEscriptor del socket por el que s eva a recibir el mensaje 
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */

struct mensaje_cola *recv_message(int s){
	uint32_t *tam;
	char *msg;
    struct mensaje_cola *res;
	res = malloc(sizeof(struct mensaje_cola));
	tam = malloc(sizeof(uint32_t));

		//Se recibe el tamaño del mensaje
		if(recv(s,tam,sizeof(uint32_t),MSG_WAITALL)<0){
            res->size=0;
            res->mensaje = NULL;
			return res;
		}
    res->size = *tam;
	
    msg = (char*)malloc(*tam);
	//Se recibe el codigo de operacion
		if(recv(s,msg,*tam,MSG_WAITALL)<0){
			res->size=0;
            res->mensaje = NULL;
			return res;
		}

    res->mensaje = msg;
	return res;
}

/**
 * Funcion que se encarga de añadir un mensaje a la cola indicada, recibe 3 argumentos: 
 * - d: Diccionario en el que se almacenan los mensajes
 * - d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - msg: Mensaje que se quiere introducir en la cola
 *   
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */

int escritura_mensaje(struct diccionario *d,char *name, struct mensaje_cola *msg){
    int error ;
    struct cola *c;
    c = dic_get(d,name,&error);
	if(msg->mensaje == NULL){
		return -1;
	}

    if(error<0){
		return -1;
    }

   if(cola_push_back(c,msg)<0){
	   return -1;
   }
   return 0;
}

/**
 * Funcion que se encarga de leer un mensaje de la cola, recibe 2 argumentos: 
 * - d: Diccionario en el que se almacenan los mensajes
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve el mensaje en caso de acierto y un mensaje que contiene un 0 en caso de que la cola este vacia
 * ya que es la forma de indicar al cliente que la cola se encontraba vacia y NULL en caso de fallo
 * */

struct mensaje_cola *lectura_mensaje(struct diccionario *d, char *name){
	int error;
    struct cola *c;
	struct mensaje_cola *msg;
    c = dic_get(d,name,&error);
	if(error<0){
		return NULL;
    }
	msg = cola_pop_front(c,&error);
	if(msg==NULL){
		struct mensaje_cola *response;
		response =  malloc(sizeof(struct mensaje_cola));
		response -> mensaje = "0";
		response -> size = 2;
		return response;
	}
		return msg; 
}

/**
 * Funcion que se encarga de mandar un mensaje al cliente correspondiente, recibe 3 argumentos: 
 * - s: Identificador del socket sobre el que se va a mandar el mensaje
 * - d: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */

int send_mensaje_get(int s_conec,struct diccionario *d, char *name){
		struct mensaje_cola *msg_put;
			if((msg_put = lectura_mensaje(d,name))==NULL){
				//Se indica que ha habido un error al leer el mensaje
				send_message(s_conec,"ERROR",6);
				return -1;
			}
			//Se envia el mensaje
			send_message(s_conec,msg_put->mensaje,msg_put->size);
		free(msg_put);
		return 0;
}

/**
 * Funcion que se encarga de desbloquear un cliente, recibe 3 argumentos: 
 * - d: Diccionario en el que se almacenan los mensajes
 * - d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve el descriptor del socket en caso de acierto y -1 en caso de fallo
 * */

int mensaje_get(struct diccionario *d, struct diccionario *d_get, char *cola){
	int error;
    struct cola *c;
	int *s_conec;
	c = dic_get(d_get,cola,&error);
	if(error<0){
		return -1;
    }
	s_conec = cola_pop_front(c,&error);
	if(error<0){
		return -1;
    }
	//Se envia el mensaje
	return send_mensaje_get(*s_conec,d,cola);
}

/**
 * Funcion que se encarga de introducir un cliente en el diccionario de clientes en espera, recibe 3 argumentos: 
 * - s_conec: Descriptor del socket
 * - d_get: Diccionario en el que se almacenan los clientes que estan esperando un mensaje
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve 0 en caso de acierto y -1 en caso de fallo
 * */
int get_bloqueante(int s_conec,struct diccionario *d_get, char * name){
	int error ;
    struct cola *c;
	int *s;
    c = dic_get(d_get,name,&error);

    if(error<0){
		return -1;
    }
	s = malloc(sizeof(int));
	*s = s_conec;
   if(cola_push_back(c,s)<0){
	   return -1;
   }
   return 0;
}

/**
 * Funcion que comprueba si hay elementos en la cola indicada, recibe 3 argumentos: 
 * - d: Diccionario en el que se quiere comprobar si hay mensajes
 * - name: nombre de la cola que se quiere crear
 * 
 * Devuelve la longitud de la cola en caso de acierto y -1 en caso de fallo
 * */
int check_elements(struct diccionario *d, char *name){
		struct cola *c;
		int *error;
		error=malloc(sizeof(int));
		c = dic_get(d,name,error);
		if(*error<0){
			return -1;
		}
		free(error);
		return cola_length(c);
} 

int main(int argc, char *argv[]) {
	int s, s_conec;
	unsigned int tam_dir;
	struct sockaddr_in dir;
	struct diccionario *d;
	struct diccionario *d_get_bloq; //Diccionario para el get bloqueante
	int opcion=1;

	if (argc!=2) {
		return -1;
	}
	if ((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		return -1;
	}
		
	/* Para reutilizar puerto inmediatamente */
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion))<0){
                return -1;
        }

	dir.sin_addr.s_addr=INADDR_ANY;
	dir.sin_port=htons(atoi(argv[1]));
	dir.sin_family=PF_INET;
	if (bind(s, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		close(s);
		return -1;
	}
	if (listen(s, 5) < 0) {
		close(s);
		return -1;
	}

	if((d = dic_create())==NULL){
		close(s);
		return -1;
	}

	if((d_get_bloq = dic_create())==NULL){
		close(s);
		return -1;
	}

	while (1) {
		tam_dir=sizeof(dir);
		
		if ((s_conec=accept(s, (struct sockaddr *)&dir, &tam_dir))<0){
			close(s);
			return -1;
		}
	//-------- AQUI ACABA EL CODIGO DEL SOCKET --------
		char *op,*b;
		char *name_cola;
		int error, size_name;

		op = malloc(2);
		//Se recibe el codigo de operacion
		if(read(s_conec,op,2)<0){
			close(s);
			return -1;
		}
		//Se recibe el tamaño del nombre de la cola
		if(recv(s_conec,&size_name,sizeof(int),MSG_WAITALL)<0){
			close(s);
			return -1;
		}
		//Se reserva tamaño para el nombre de la cola
		name_cola=malloc(size_name);
		//Se recibe el nombre de la cola
		if(recv(s_conec,name_cola,size_name,MSG_WAITALL)<0){
			close(s);
			return -1;
		}
		//Se recibe el identifcador de get bloqueante
		b = malloc(2);
		if(recv(s_conec,b,2,MSG_WAITALL)<0){
			close(s);
			return -1;
		}

		switch (*op){
		case '0': //Crear Cola
			free(op);
			error = crea_cola(d,d_get_bloq,name_cola);
			send_response(s_conec,error);
			break;
		case '1': //Destruir Cola
			free(op);
			error = elimina_cola(d,d_get_bloq,name_cola);
			send_response(s_conec,error);
			break;
		case '2': //put
			free(op);
			struct mensaje_cola *mensaje;
			mensaje = recv_message(s_conec);
			error = escritura_mensaje(d,name_cola,mensaje);	
			send_response(s_conec,error);
			mensaje_get(d,d_get_bloq,name_cola); //Se envia el mensaje al get bloqueante	
			break;
		case '3': //get
			free(op);
			if(strncmp(b,"1",1)==0 && check_elements(d,name_cola)<=0){
				get_bloqueante(s_conec,d_get_bloq,name_cola);
				break;
			}
			send_mensaje_get(s_conec,d,name_cola);
			break;
		default:
			free(op);
			free(name_cola);
			free(b);
			close(s_conec);
			break;
		}
		//Si el get no es bloqueante se cierra el descriptor
		if(strncmp(b,"1",1)!=0){
			close(s_conec);
		}
		free(b);
	}
	close(s);
	return 0;
}
