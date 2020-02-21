//
// IMPLEMENTACIÓN DEL TIPO DE DATOS QUE IMPLEMENTA UNA COLA.
// NO PUEDE MODIFICAR ESTE FICHERO.
// NO ES NECESARIO QUE CONOZCA LOS DETALLES DE LA IMPLEMENTACIÓN PARA USARLO.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cola.h"

// definición de tipos
struct entrada {
    const void *valor;
    struct entrada *sig;
};
#define MAGIC (('Q' << 24) + ('U' << 16) + ('E' << 8) + 'U')
struct cola {
    int magic;
    int nentradas;
    struct entrada *prim;
    struct entrada *ult;
};

// especificación de funciones internas
static void itera_cola(const struct cola *c, func_entrada_cola_t iter, int destroy);
static int check_cola(const struct cola *c);
    
// implementación de funciones externas
struct cola *cola_create(void) {
    struct cola *c = malloc(sizeof(struct cola));
    if (!c) return NULL;
    c->magic=MAGIC;
    c->nentradas=0;
    c->prim=c->ult=NULL;
    return c;
}
int cola_destroy(struct cola *c, func_entrada_cola_t liberar_entrada){
    if (check_cola(c)) return -1;
    if (liberar_entrada)
        itera_cola(c, liberar_entrada, 1);
    c->magic=0;
    free(c);
    return 0;
}
int cola_visit(const struct cola *c, func_entrada_cola_t visitar_entrada){
    if (check_cola(c)) return -1;
    if (visitar_entrada)
        itera_cola(c, visitar_entrada, 0);
    return 0;
}
int cola_push_back(struct cola *c, const void *valor){
    if (check_cola(c))  return -1;
    struct entrada *e;
    e=malloc(sizeof(struct entrada));
    if (!e) return -1;
    c->nentradas++;
    e->valor=valor;
    e->sig=NULL;
    if (c->ult) c->ult->sig=e;
    c->ult=e;
    if (!c->prim) c->prim=c->ult;
    return 0;
}
void * cola_pop_front(struct cola *c, int *error){
    int err=0;
    void *valor;
    if ((check_cola(c)) || !c->nentradas) err=-1;
    else {
        struct entrada *prim=c->prim;
        c->nentradas--;
        valor = (void *)prim->valor;
        c->prim=prim->sig;
	free(prim);
        if (!c->prim) c->ult=NULL;
    }
    if (error) *error=err;
    return (err==-1?NULL:valor);
}
int cola_length(const struct cola *c){
    if (check_cola(c))  return -1;
    return c->nentradas;
}

// implementación de funciones internas
static void itera_cola(const struct cola *c, func_entrada_cola_t iter, int destroy){
    struct entrada *e, *sig;
    for (e=c->prim; e; e=sig) {
        iter((void *)e->valor);
        sig=e->sig;
	if (destroy) free(e);
    }
}
static int check_cola(const struct cola *c){
    int res=0;
    if (c==NULL || c->magic!=MAGIC){
        res=-1; fprintf(stderr, "la cola especificada no es válida\n");
    }
    return res;
}
