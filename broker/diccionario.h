//
// DEFINICIÓN DEL TIPO DE DATOS QUE IMPLEMENTA UN DICCIONARIO.
// NO PUEDE MODIFICAR ESTE FICHERO.
// DEBE USAR OBLIGATORIAMENTE ESTA ESTRUCTURA DE DATOS PARA RESOLVER
// LA PRÁCTICA.
// PUEDE VER UN EJEMPLO DE USO EN demo_dic.c.

/*
 * Diccionario: Colección de entradas -> [clave (de tipo string) y valor]
 *
 * Almacena referencias (no copias) de cada clave y valor.
 *
 * Al destruir un diccionario, su implementación invoca por cada entrada
 * existente en el mismo la función de liberación especificada como parámetro
 * para permitir que el usuario de esta colección pueda liberar,
 * si lo considera oportuno, la información asociada a cada clave y valor.
 *
 * Al eliminar una entrada, también se invoca la función especificada
 * como parámetro, si no es NULL, para esa entrada.
 */

#ifndef _DICCIONARIO_H
#define _DICCIONARIO_H      1

// Tipo opaco para ocultar la implementación
struct diccionario;

// Tipo de datos para una función que visita una entrada
typedef void (*func_entrada_dic_t) (char *clave, void *valor);

// Crea un diccionario.
// Devuelve una referencia a un diccionario o NULL en caso de error.
struct diccionario *dic_create(void);

// Destruye el diccionario especificado. Si tiene todavía entradas
// se invocará la función especificada como parámetro por cada una de ellas
// pasando como argumentos a la misma la clave y valor de la entrada.
// Si la aplicación no está interesada en ser notificada de las entradas
// existentes, debe especificar NULL en el parámetro de esta función.
// Devuelve 0 si OK y -1 si error.
int dic_destroy(struct diccionario *d, func_entrada_dic_t liberar_entrada);

// Permite recorrer todas las entradas de un diccionario.
// Se invocará la función especificada como parámetro por cada una de ellas
// pasando como argumentos a la misma la clave y valor de la entrada.
// Devuelve 0 si OK y -1 si error.
int dic_visit(const struct diccionario *d, func_entrada_dic_t visitar_entrada);

// Inserta en el diccionario indicado la entrada especificada por la clave
// y el valor. Almacena referencias (y no copias) de la clave y el valor.
// Devuelve 0 si OK y -1 si error (p.e. si duplicada).
int dic_put(struct diccionario *d, const char *clave, const void *valor);

// Retorna el valor asociado a una clave.
// Dado que cualquier valor es válido, devuelve en el tercer parámetro
// si se ha producido un error: 0 si OK y -1 si error.
void * dic_get(const struct diccionario *d, const char *clave, int *error);

// Elimina una entrada dada su clave. Si se recibe un valor distinto
// de NULL en el tercer parámetro, se invoca esa función como parte
// de la eliminación de la entrada.
// Devuelve 0 si OK y -1 si error.
int dic_remove_entry(struct diccionario *d, const char *clave, func_entrada_dic_t liberar_entrada);

#endif // _DICCIONARIO_H
