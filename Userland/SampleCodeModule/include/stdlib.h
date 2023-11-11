#ifndef _STDLIB_H
#define _STDLIB_H
#include <stdint.h>
#include <types.h>
#include <syscalls.h>

/**
 * @brief Crea un proceso
 * @param code: Puntero a la funcion que se desea ejecutar
 * @param args: Arreglo de cadenas de caracteres con los argumentos de la
 * funcion
 * @param name: Nombre del proceso
 * @param priority: Prioridad del proceso
 */
int createProcess(int16_t parentPid, ProcessCode code, char ** args, char * name, uint8_t priority);
/**
 * @brief Convierte un arreglo de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @return Numero entero en formato decimal
 */
int atoi(char * s);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato decimal
 */
int strtoi(char * s, char ** end);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato
 * hexadecimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato hexadecimal
 */
int strtoh(char * s, char ** end);

/**
 * @brief Convierte un numero en una base en una cadena de caracteres
 * @param n: Numero que se desea convertir
 * @param buffer: Buffer para ir guardando la conversion
 * @param base: Base del numero que se desea convertir
 * @return Longitud del buffer (numero que se convirtio en cadena de caracteres)
 */
int itoa(uint64_t n, char * buffer, int base);
#endif