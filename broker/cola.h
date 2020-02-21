//
// DEFINICIÓN DEL TIPO DE DATOS QUE IMPLEMENTA UNA COLA.
// NO PUEDE MODIFICAR ESTE FICHERO.
// DEBE USAR OBLIGATORIAMENTE ESTA ESTRUCTURA DE DATOS PARA RESOLVER
// LA PRÁCTICA.
// PUEDE VER UN EJEMPLO DE USO EN demo_cola.c.

/*
 * Cola: Lista de entradas gestionada con un esquema FIFO.
 *
 * Almacena una referencia (no copia) de cada valor.
 *
 * Al destruir una cola, su implementación invoca la función
 * de liberación especificada como parámetro por cada entrada existente
 * en el mismo para permitir que el usuario de esta colección pueda liberar,
 * si lo considera oportuno, la información asociada a cada clave y valor.
 *
 * Al eliminar una entrada, también se invoca la función especificada
 * como parámetro, si no es NULL, para esa entrada.
 */

#ifndef _COLA_H
#define _COLA_H      1

// Tipo opaco para ocultar la implementación
struct cola;

// Tipo de datos para una función que visita una entrada
typedef void (*func_entrada_cola_t) (void *valor);

// Crea una cola.
// Devuelve una referencia a una cola o NULL en caso de error.
struct cola *cola_create(void);

// Destruye la cola especificada. Si tiene todavía entradas
// se invocará la función especificada como parámetro por cada una de ellas
// pasando como argumento a la misma el valor asociado a la entrada.
// Si la aplicación no está interesada en ser notificada de las entradas
// existentes, debe especificar NULL en el parámetro de esta función.
// Devuelve 0 si OK y -1 si error.
int cola_destroy(struct cola *c, func_entrada_cola_t liberar_entrada);

// Permite recorrer todas las entradas de una cola.
// Devuelve 0 si OK y -1 si error.
int cola_visit(const struct cola *c, func_entrada_cola_t visitar_entrada);

// Inserta al final de la cola un nuevo elemento.
// Almacena una referencia (y no copia) del valor.
// Devuelve 0 si OK y -1 si error.
int cola_push_back(struct cola *c, const void *valor);

// Extrae el primer elemento de la cola.
// Dado que cualquier valor es válido, devuelve en el segundo parámetro
// si se ha producido un error: 0 si OK y -1 si error.
void * cola_pop_front(struct cola *c, int *error);

// Devuelve el nº de elementos en la cola, -1 si error.
int cola_length(const struct cola *c);

#endif // _COLA_H
