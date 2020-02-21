//
// IMPLEMENTACIÓN DEL TIPO DE DATOS QUE IMPLEMENTA UN DICCIONARIO.
// NO PUEDE MODIFICAR ESTE FICHERO.
// NO ES NECESARIO QUE CONOZCA LOS DETALLES DE LA IMPLEMENTACIÓN PARA USARLO.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "diccionario.h"

// definición de tipos
struct entrada {
    const char *clave;
    const void *valor;
};
#define MAGIC (('D' << 24) + ('I' << 16) + ('C' << 8) + 'T')
struct diccionario {
    int magic;
    int nentradas;
    struct entrada *coleccion;
};

// especificación de funciones internas
static int busca_entrada(const struct diccionario *d, const char *clave);
static void itera_dic(const struct diccionario *d,  func_entrada_dic_t iter);
static int check_dic(const struct diccionario *d);
    
// implementación de funciones externas
struct diccionario *dic_create(void) {
    struct diccionario *d = malloc(sizeof(struct diccionario));
    if (!d) return NULL;
    d->magic=MAGIC;
    d->nentradas=0;
    d->coleccion=NULL;
    return d;
}
int dic_destroy(struct diccionario *d, func_entrada_dic_t liberar_entrada){
    if (check_dic(d)) return -1;
    if (liberar_entrada)
        itera_dic(d, liberar_entrada);
    free(d->coleccion);
    d->magic=0;
    free(d);
    return 0;
}
int dic_visit(const struct diccionario *d, func_entrada_dic_t visitar_entrada){
    if (check_dic(d)) return -1;
    if (visitar_entrada)
        itera_dic(d, visitar_entrada);
    return 0;
}
int dic_put(struct diccionario *d, const char *clave, const void *valor) {
    if (check_dic(d) || !clave || busca_entrada(d, clave)!=-1)  return -1;
    d->nentradas++;
    d->coleccion=realloc(d->coleccion, d->nentradas*sizeof(struct entrada));
    if (!d->coleccion) return -1;
    struct entrada e = {clave, valor};
    d->coleccion[d->nentradas-1] =  e;
    return 0;
}
void * dic_get(const struct diccionario *d, const char *clave, int *error){
    int ne, err=0;
    if ((check_dic(d)) || (!clave || (ne=busca_entrada(d, clave))==-1)) err=-1;
    if (error) *error=err;
    return (err==-1?NULL:(void *)d->coleccion[ne].valor);
}
int dic_remove_entry(struct diccionario *d, const char *clave,
        func_entrada_dic_t liberar_entrada) {
    int ne;
    if ((check_dic(d)) || !clave || (ne=busca_entrada(d, clave))==-1) return -1;
    if (liberar_entrada)
        liberar_entrada((char *)d->coleccion[ne].clave,
            (void *)d->coleccion[ne].valor);
    d->nentradas--;
    d->coleccion[ne] = d->coleccion[d->nentradas];
    d->coleccion=realloc(d->coleccion, d->nentradas*sizeof(struct entrada));
    return 0;
}

// implementación de funciones internas
static int busca_entrada(const struct diccionario *d, const char *clave){
    int i;

    if (!clave) return -1;
    for (i=0; (i<d->nentradas && strcmp(clave, d->coleccion[i].clave)!=0); i++);
    return (i==d->nentradas?-1:i);
}
static void itera_dic(const struct diccionario *d,  func_entrada_dic_t iter){
    for (int i=0; i<d->nentradas; i++)
        iter((char *)d->coleccion[i].clave, (void *)d->coleccion[i].valor);
}
static int check_dic(const struct diccionario *d){
    int res=0;
    if (d==NULL || d->magic!=MAGIC){
        res=-1; fprintf(stderr, "el diccionario especificado no es válido\n");
    }
    return res;
}
