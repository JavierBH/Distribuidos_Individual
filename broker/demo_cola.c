// EJEMPLO DE USO DEL TIPO DE DATOS COLA
#include <stdio.h>
#include <stdlib.h>
#include "cola.h"

struct punto2D {
    int x;
    int y;
};

void libera_punto2D(void *v) {
    // libera la estructura punto2D reservada mediante malloc
    free(v);
}
void inserta_punto2D(struct cola *c) {
    int x,y;
    struct punto2D *p;
    printf("Introduzca en una línea las coordenadas X e Y del punto a insertar:\n");
    // la opción %m realiza un malloc internamente
    scanf("%d%d", &x, &y); 
    p = malloc(sizeof(struct punto2D));
    p->x = x; p->y = y;
    cola_push_back(c, p);
}
void extrae_punto2D(struct cola *c) {
    struct punto2D *p;
    int error;
    p=cola_pop_front(c, &error);
    if (error < 0)
        printf("cola vacía\n");
    else
        printf("punto extraído: (%d,%d)\n", p->x, p->y);
    free(p);
}
void imprime_punto2D(void *v) {
    struct punto2D *p = v;
    printf("punto: (%d,%d)\n", p->x, p->y);
}
void imprime_int(void *v) {
    printf("número %ld\n", (long)v);
}
int main(int argc, char *argv[]) {
    struct cola *cola;

    // ejemplo que guarda en cada elemento de la lista una referencia
    // a un punto 2D
    cola = cola_create();
    inserta_punto2D(cola);
    inserta_punto2D(cola);
    inserta_punto2D(cola);
    printf("contenido de la cola:\n");
    cola_visit(cola, imprime_punto2D);
    extrae_punto2D(cola);
    extrae_punto2D(cola);
    printf("contenido de la cola:\n");
    cola_visit(cola, imprime_punto2D);
    cola_destroy(cola, libera_punto2D);

    // ejemplo que guarda en cada elemento de la lista directamente
    // un valor entero aleatorio (no una referencia)
    cola = cola_create();
    cola_push_back(cola, (void *)(long)rand());
    cola_push_back(cola, (void *)(long)rand());
    cola_push_back(cola, (void *)(long)rand());
    printf("contenido de la cola:\n");
    cola_visit(cola, imprime_int);
    int error;
    int p=(long)cola_pop_front(cola, &error);
    if (error < 0)
        printf("cola vacía\n");
    else
        printf("número extraído %d\n", p);
    printf("contenido de la cola:\n");
    cola_visit(cola, imprime_int);
    cola_destroy(cola, NULL); // no hay nada que liberar

    return 0;
}
