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

#define TAM 1024

   // Funcion que libera la estructura de datos de la cola

void libera_cola(void *c){
    free(c);
}

 /*Esta funcion se ejecuta en el caso de que se quiera crear una cola
    Añade la cola al diccionario, asignandole un valor de la cola, ya que se encuentra vacia. DE la siguiente manera:
    Name_cola : struct cola*/

int crea_cola(struct diccionario *d, char *name){
 if (dic_put(d, name, cola_create()) < 0){
        fprintf(stderr, "Cola duplicada \n");
		return -1;
	}
	return 0;
}

/*Esta funcion se ejecuta cuando llega una operacion de eliminar cola.
Busca en el diccionario la cola que se quiere eliminar y si existe la borra, junto a sus mensajes.
En caso de que no exista da un error*/

int elimina_cola(struct diccionario *d, char *name){
	if (dic_remove_entry(d,name, libera_cola) < 0){
    	    fprintf(stderr, "Cola no existente\n");
			return -1;
	}
	return 0;
}

    //Esta funcion se ejectua cuando lelga la orden de añadir un mensaje a la cola corresponiente

void *escritura_mensaje(struct diccionario *d,char *cola, void *mensaje){
    int error = 0;
    struct cola *c;
    c = dic_get(d,cola,&error);
    if(error<0){
        fprintf(stderr, "No existe la cola solicitida duplicada \n");
    }
   if(cola_push_back(c,mensaje)<0){
        fprintf(stderr, "Error al introducir el mensaje en la cola solicitada \n");
   }
   free(c);
}

 //   Esta funcion se ejectua cuando llega la orden de leer un mensaje

char *lectura_mensaje(struct diccionario *d, char *cola){
	int error = 0;
    struct cola *c;
    c = dic_get(d,cola,&error);
    if(error<0){
        fprintf(stderr, "No existe la cola solicitida duplicada \n");
    }
	return cola_pop_front(c,&error);
}

//Funcion que devuelve al cliente 0 si la operacion ha sido correcta y -1 si ha sido incrorecta.  
int send_response(int s,int code){
	char *response;
	if(code==0){
		response = "OK";
	}else{
		response = "FAIL";
	}
	if(write(s,response,strlen(response))<0){
		perror("Error en el envio del codigo");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int s, s_conec;
	unsigned int tam_dir;
	struct sockaddr_in dir, dir_cliente;
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
		int *op;
		char *name_cola;
		char *msg;
		int error;
		/*
		El orden de llegada es el siguiente:
		- Se recibe un int indicando el numero de operacion
		- SE recibe la longitud de la cola
		- Se recibe el nombre de la cola
		- Se recibe la longitud del mensaje
		- Se recibe el mensaje (se recibe un 0 en caso de que no haya mensaje se recibira un 0)		
		readv(socket descriptor, iov structure, number of buffers expected) 
		*/

		op = (int*)malloc(sizeof(int));
		//Se recibe el codigo de operacion
		if(read(s_conec,op,1)<0){
			perror("Error en la llegada");
			close(s);
			return -1;
		}

		//Se reserva espacio para una cadena 4096 caracteres
		name_cola= (char *)malloc(TAM*sizeof(char));
		//Se leen 4096 caracteres, si el tamaño de la cola es mayor se utilizawr realloc
		while(read(s_conec,name_cola,TAM)==1024){
			name_cola=realloc(name_cola,TAM*sizeof(char)+sizeof(name_cola));
		}
		//Nota: op tiene el valor en ASCII
		switch (*op){
		case '0': //Crear Cola
			free(op);
			error = crea_cola(d,name_cola);	
			free(name_cola);
			send_response(s_conec,error);
			break;
		case '1': //Destruir Cola
			free(op);
			elimina_cola(d,name_cola);
			free(name_cola);
			break;
		case '2': //put
			/* code */
			break;
		case '3': //get
			/* code */
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
