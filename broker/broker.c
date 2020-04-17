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
struct mensaje_cola{
	int size;
	char * mensaje;
};

void libera_cola(void *v){
	free(v);
}

void libera_cola_dic(char *c, void *v){
 	struct cola *cola;
	cola = v;
	cola_destroy(cola,libera_cola);
	 free(c);
	
}

 /*Esta funcion se ejecuta en el caso de que se quiera crear una cola
    Añade la cola al diccionario, asignandole un valor de la cola, ya que se encuentra vacia. DE la siguiente manera:
    Name_cola : struct cola
	*/

int crea_cola(struct diccionario *d, char *name){
	struct cola *c;
	if((c = cola_create())==NULL){
		return -1;
	}
 	if(dic_put(d, name, c) < 0){
		return -1;
	}
	return 0;
}

/*Esta funcion se ejecuta cuando llega una operacion de eliminar cola.
Busca en el diccionario la cola que se quiere eliminar y si existe la borra, junto a sus mensajes.
En caso de que no exista da un error*/

int elimina_cola(struct diccionario *d, char *name){
	if (dic_remove_entry(d,name, libera_cola_dic) < 0){
			return -1;
	}
	return 0;
}

//Fucnion que recive el mensaje
struct mensaje_cola *recv_message(int s){
	int *tam;
	char *msg;
    struct mensaje_cola *res;
	res = malloc(sizeof(struct mensaje_cola));
	tam = malloc(sizeof(uint32_t));
		//Se recibe el tamaño del mensaje
		if(read(s,tam,sizeof(uint32_t))<0){
            res->size=0;
            res->mensaje = NULL;
			return res;
		}

    res->size = *tam;
	
    msg = (char*)malloc(*tam);
	//Se recibe el codigo de operacion
		if(read(s,msg,*tam)<0){
			res->size=0;
            res->mensaje = NULL;
			return res;
		}

    res->mensaje = msg;
	return res;
}

    //Esta funcion se ejectua cuando llega la orden de añadir un mensaje a la cola corresponiente

int escritura_mensaje(struct diccionario *d,char *cola, struct mensaje_cola *msg){
    int error ;
    struct cola *c;
    c = dic_get(d,cola,&error);
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

 //   Esta funcion se ejectua cuando llega la orden de leer un mensaje

struct mensaje_cola *lectura_mensaje(struct diccionario *d, char *cola){
	int error;
    struct cola *c;
    c = dic_get(d,cola,&error);
	if(error<0){
		return NULL;
    }
	return cola_pop_front(c,&error);
}

int recv_tam(int s){
	int tam;
	if(read(s,&tam,sizeof(int))<0){
			close(s);
			return -1;
		}
	return tam;
}

int delete_get_cola(struct diccionario *d,char *cola){
	char *msg;
	int error;
    struct cola *c;
	int *s_conec;
    c = dic_get(d,cola,&error);
	if(error<0){
		return -1;
    }
	
	msg = "ERROR";
	//Se manda un mensaje a cada uno de los clientes que estaban esperando
	while((s_conec = cola_pop_front(c,&error))!= NULL && error>0 ){
		send_message(*s_conec,msg,strlen(msg));
		close(*s_conec);
		free(s_conec);
	}
	//Se elimina la cola
	return elimina_cola(d, cola);
}

//Funcion encargada de mandar el mensje del get
int send_mensaje_get(int s_conec,struct diccionario *d, char *name_cola){
		struct mensaje_cola *msg_put;
			if((msg_put = lectura_mensaje(d,name_cola))==NULL){
				send_message(s_conec,NULL,1);
				return -1;
			}
			send_message(s_conec,msg_put->mensaje,msg_put->size);
		free(msg_put);
		return 0;
}

//Funcion que desbloquea uno de los clientes esperando a leer el mensaje de la cola indicada
int mensaje_get(struct diccionario *dic_mensajes, struct diccionario *dic_get_b, char *cola){
	int error;
    struct cola *c;
	int *s_conec;
	c = dic_get(dic_get_b,cola,&error);
	if(error<0){
		return -1;
    }
	s_conec = cola_pop_front(c,&error);
	if(error<0){
		//lala
        //No hay elementos en la cola, por lo que no es necesario mandar el mensaje
		return 0;
    }
	//Se envia el mensaje
	return send_mensaje_get(*s_conec,dic_mensajes,cola);
}


int get_bloqueante(int s_conec,struct diccionario *d, char * cola){
	int error ;
    struct cola *c;
	int *s;
    c = dic_get(d,cola,&error);

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

//Comprueba que hay elementos en la cola indicada
int check_elements(struct diccionario *d, char *name_cola){
		struct cola *c;
		int *error;
		error=malloc(sizeof(int));
		c = dic_get(d,name_cola,error);
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

		b = malloc(2);
		if(recv(s_conec,b,2,MSG_WAITALL)<0){
			close(s);
			return -1;
		}

		switch (*op){
		case '0': //Crear Cola
			free(op);
			error = crea_cola(d,name_cola);
			send_response(s_conec,error);
			crea_cola(d_get_bloq,name_cola);	//Se introduce la cola en el diccionario para el get bloqueante	
			break;
		case '1': //Destruir Cola
			free(op);
			error = elimina_cola(d,name_cola);
			send_response(s_conec,error);
			delete_get_cola(d_get_bloq,name_cola); //Se elimina la cola del get bloqueante
			break;
		case '2': //put
			free(op);
			struct mensaje_cola *mensaje;
			mensaje = recv_message(s_conec);
			error = escritura_mensaje(d,name_cola,mensaje);	
			// Se envia la respuesta del mensaje
			send_response(s_conec,error);
			mensaje_get(d,d_get_bloq,name_cola); //Se envia el mensaje al get bloqueante	
			break;
		case '3': //get
			free(op);
			fprintf(stderr,"%d\n",strncmp(b,"0",1));
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
		if(strncmp(b,"1",1)!=0){
			close(s_conec);
		}
		free(b);
	}
	close(s);
	return 0;
}
