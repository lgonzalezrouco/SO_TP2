#ifndef _U_STDLIB_H
#define _U_STDLIB_H

#include <stdint.h>
#include <syscalls.h>
#include <types.h>

/**
 * @brief Crea un proceso
 * @param code: Puntero a la funcion que se desea ejecutar
 * @param args: Arreglo de cadenas de caracteres con los argumentos de la
 * funcion
 * @param name: Nombre del proceso
 * @param isForeground: Si es foreground o no
 */
int createProcess(ProcessCode code, char **args, char *name, uint8_t isForeground);

/**
 * @brief Convierte un arreglo de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @return Numero entero en formato decimal
 */
int atoi(char *s);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato decimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato decimal
 */
int strtoi(char *s, char **end);

/**
 * @brief Convierte una cadena de caracteres en numero entero en formato
 * hexadecimal
 * @param s: Cadena de caracteres
 * @param end: Puntero al final de la cadena de caracteres
 * @return Numero entero en formato hexadecimal
 */
int strtoh(char *s, char **end);

/**
 * @brief Convierte un numero en una base en una cadena de caracteres
 * @param n: Numero que se desea convertir
 * @param buffer: Buffer para ir guardando la conversion
 * @param base: Base del numero que se desea convertir
 * @return Longitud del buffer (numero que se convirtio en cadena de caracteres)
 */
int itoa(uint64_t n, char *buffer, int base);

/**
 * @brief Convierte una cadena de caracteres a un puntero
 * @param buffer: Cadena de caracteres
 * @return Puntero
 */
void *stringToPtr(char *buffer);

/**
 * @brief Convierte un numero entero a una cadena de caracteres en una base
 * @param num: Numero que se desea convertir
 * @param base: Base del numero que se desea convertir
 * @param buffer: Buffer para ir guardando la conversion
 */
int intToBase(unsigned long long num, int base, char *buffer);

/**
 * @brief Convierte un numero entero a una cadena de caracteres en base 10
 * @param num: Numero que se desea convertir
 * @param buffer: Buffer para ir guardando la conversion
 */
int intToString(unsigned long long num, char *buffer);
#endif