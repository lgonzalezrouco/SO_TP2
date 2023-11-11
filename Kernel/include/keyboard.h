#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#define EOF -1

void initializeKeyboardDriver();

/* Handler de interrupciones de teclado */
void keyboardHandler();

/* Devuelve el valor ascii del ultimo caracter en el buffer de teclado */
int8_t getAscii();

/* Devuelve el scancode del ultimo caracter en el buffer de teclado */
int8_t getScancode();
#endif
