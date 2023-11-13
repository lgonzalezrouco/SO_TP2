#ifndef _LIB_H
#define _LIB_H

#include <stdint.h>

/**
 * @brief  Escribe 'character' en una direccion de memoria 'length' veces
 * @param  destination: Lugar a escribir
 * @param  character: Elemento a copiar
 * @param  length: Cantidad de veces que se desea escrbir el valor
 */
void *memset(void *destination, int32_t character, uint64_t length);

/**
 * @brief Copia a 'destination' 'length' caracteres de source
 * @param  destination: Direccion de origen
 * @param  source: Fuente
 * @param  length: Cantidad de bytes a copiar
 */
void *memcpy(void *destination, const void *source, uint64_t length);

/**
 * @brief Convierte un entero a string
 * @note  Actualmente no trabaja con negativos
 * @param  n: Numero a convertir
 * @param  buffer: Direccion donde se quiere copiar la respuesta
 * @param  base: Base numerica
 * @return Longitud del string generado
 */
int itoa(uint64_t n, char *buffer, int base);

/**
 * @brief Convierte un string a entero base 10
 * @param  s: String a convertir
 * @param  end: Puntero a la ultima direccion del string
 * @return Entero a partir del string
 */
int strtoi(char *s, char **end);

/**
 * @brief Devuelve el scancode de la ultima tecla presionada
 * @return Scancode
 */
char getKeyPressed();

/**
 * @brief Permite obtener el fabricante del cpu
 * @param  result: Puntero donde se quiere guardar el fabricante del cpu
 * @return Direccion donde se escribio el fabricante
 */
char *cpuVendor(char *result);

/**
 * @brief Devuelve la hora actual en formato de 24hs
 * @param  h: Horas
 * @param  m: Minutos
 * @param  s: Segundos
 */
void getTime(uint8_t *h, uint8_t *m, uint8_t *s);

/**
 * @brief Reproduce un sonido en el speaker
 * @param  freq: Frecuencia del sonido
 */
void startSound(uint64_t freq);

/**
 * @brief Apaga el speaker
 */
void stopSound();

/**
 * @brief Realiza un snapshot de los registros en un momento arbitrario
 */
void saveRegisters();

/**
 * @brief  Detiene la ejecucion del programa hasta que se produzca una
 * interrupcion
 */
void _hlt();

/**
 * @brief Copia el strinct src en dest
 *
 * @param dest: String destino
 * @param src: String a copiar
 */
void strcpy(char *dest, char *src);

/**
 * @brief Compara dos strings
 *
 * @param s1: String 1
 * @param s2: String 2
 * @return 0 si son iguales, 1 si s1 > s2, -1 si s1 < s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Devuelve la longitud de un string
 *
 * @param str: String a medir
 * @return Longitud del string
 */
unsigned int strlen(char *str);

/**
 * @brief Devuelve la longitud de un array de strings
 *
 * @param array: Array de strings a medir
 * @return Longitud del array
 */
unsigned int array_strlen(char **array);

/**
 * @brief Fuerza un timer
 */
void forceTimerTick();

/**
 * @brief Fuerza un cambio de proceso
 *
 * @param RSP: Puntero al stack del proceso
 */
void forceChangeOfProcess(uint64_t *RSP);

int criticalRegion(int *mutex);

void setBit(uint8_t *byte, int bitNumber);

uint64_t pow(uint32_t base, uint32_t exp);

uint32_t Log2n(uint32_t n);

char getBit(uint8_t *byte, int bitNumber);

void clearBit(uint8_t *byte, int bitNumber);

#endif
