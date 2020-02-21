// EJEMPLO DE USO DEL TIPO DE DATOS DICCIONARIO
#include <stdio.h>
#include <stdlib.h>
#include "diccionario.h"

struct persona {
    char *nombre;
    int edad;
    // ....
};

void libera_persona(char *c, void *v) {
    // libera la clave reservada por la opción %m de scanf
    free(c);
    // libera la estructura persona reservada mediante malloc
    free(v);
}
void lee_persona(struct diccionario *d) {
    char *nombre=NULL;
    int edad;
    struct persona *p;
    printf("Introduzca en una línea el nombre de la persona y su edad:\n");
    // la opción %m realiza un malloc internamente
    scanf("%ms%d", &nombre, &edad); 
    p = malloc(sizeof(struct persona));
    p->nombre = nombre;
    p->edad = edad;
    if (dic_put(d, p->nombre, p) < 0)
        fprintf(stderr, "persona duplicada\n");
}
void elimina_persona(struct diccionario *d) {
    char *nombre=NULL;
    printf("Introduzca en una línea el nombre de la persona a eliminar:\n");
    // la opción %m realiza un malloc internamente
    scanf("%ms", &nombre); 
    if (dic_remove_entry(d, nombre, libera_persona) < 0)
        fprintf(stderr, "persona no existente\n");
    free(nombre);
}
void datos_persona(struct diccionario *d) {
    char *nombre=NULL;
    struct persona *p;
    int error;
    printf("Introduzca en una línea el nombre de la persona cuyos datos se quieren conocer:\n");
    // la opción %m realiza un malloc internamente
    scanf("%ms", &nombre); 
    p=dic_get(d, nombre, &error);
    if (error < 0)
        fprintf(stderr, "persona no existente\n");
    else
        printf("edad %d\n", p->edad);
    free(nombre);
}
void imprime_persona(char *c, void *v) {
    struct persona *p = v;
    printf("nombre %s edad %d\n", c, p->edad);
}
int main(int argc, char *argv[]) {
    struct diccionario *dic;

    dic = dic_create();
    lee_persona(dic);
    lee_persona(dic);
    lee_persona(dic);
    printf("Contenido del diccionario:\n");
    dic_visit(dic, imprime_persona);
    elimina_persona(dic);
    printf("Contenido del diccionario:\n");
    dic_visit(dic, imprime_persona);
    datos_persona(dic);
    dic_destroy(dic, libera_persona);
    return 0;
}
