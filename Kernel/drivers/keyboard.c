// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <lib.h>
#include <scheduler.h>
#include <semaphores.h>
#include <stdint.h>
#include <time.h>
#include <video.h>

#define BUFFER_CAPACITY 64   /* Longitud maxima del vector _buffer */
#define LEFT_CONTROL    29   /* Scancode para la tecla control izquierda */
#define LEFT_SHIFT      42   /* Scancode para la tecla shift izquierda */
#define RELEASED        0x80 /* Mascara para detectar si una tecla fue liberada */
#define SHIFTED         0x80 /* Mascara para detectar si una tecla fue presionada con shift */
#define _C              0x2E /* Scancode para la tecla c */
#define _R              0x13 /* Scancode para la tecla r */
#define _D              0x20 /* Scancode para la tecla d */

static uint8_t _bufferStart = 0;              /* Indice del comienzo de la cola */
static uint8_t _bufferSize = 0;               /* Longitud de la cola */
static int8_t _buffer[BUFFER_CAPACITY] = {0}; /* Vector ciclico que guarda las teclas que se van leyendo del teclado */

static const char charHexMap[] = /* Mapa de scancode a ASCII */
    {0,    0,   '1', '2',  '3', '4', '5', '6', '7',  '8', '9', '0', '-', '=', '\b', ' ', 'q', 'w', 'e', 'r',
     't',  'y', 'u', 'i',  'o', 'p', '[', ']', '\n', 0,   'a', 's', 'd', 'f', 'g',  'h', 'j', 'k', 'l', ';',
     '\'', 0,   0,   '\\', 'z', 'x', 'c', 'v', 'b',  'n', 'm', ',', '.', '/', 0,    '*', 0,   ' '};

static const char shiftedCharHexMap[] = /* Mapa de scancode con shift a ASCII */
    {0,   0,   '!', '@', '#', '$', '%', '^', '&',  '*', '(', ')', '_', '+', '\b', ' ', 'Q', 'W', 'E', 'R',
     'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,   'A', 'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L', ';',
     '"', 0,   0,   '|', 'Z', 'X', 'C', 'V', 'B',  'N', 'M', '<', '>', '?', 0,    '*', 0,   ' '};

static uint8_t _ctrl = 0;  /* Flag para detectar si se presiono ctrl */
static uint8_t _shift = 0; /* Flag para detectar si se presiono shift */

/**
 * @brief  Obtiene el indice del elemento en la cola dado un corrimiento
 * @param  offset: corrimiento
 * @return Indice del elemento en la cola
 */
static int getBufferIndex(int offset) {
	return (_bufferStart + offset) % (BUFFER_CAPACITY);
}
static void writeKey(int8_t key);

void initializeKeyboardDriver() {
	semInit("keyboard", 0);
}

void keyboardHandler() {
	uint8_t key = getKeyPressed();
	if (!(key & RELEASED)) {
		if (key == LEFT_CONTROL)
			_ctrl = 1;
		else if (key == LEFT_SHIFT)
			_shift = 1;
		else if (_ctrl) {
			switch (key) {
				case _C:
					_bufferStart = 0;
					_bufferSize = 0;
					killForegroundProcess();
					break;
				case _R:
					saveRegisters();
					break;
				case _D:
					if (_bufferSize < BUFFER_CAPACITY - 1)
						writeKey(EOF);
					break;
				default:
					break;
			}
		} else if (_bufferSize < BUFFER_CAPACITY - 1) {
			if (_shift)
				key = SHIFTED | key;
			writeKey(key);
		}
	} else {
		if (key == (LEFT_CONTROL | RELEASED))
			_ctrl = 0;
		else if (key == (LEFT_SHIFT | RELEASED))
			_shift = 0;
	}
}

static void writeKey(int8_t key) {
	if (((key & 0x7F) < sizeof(charHexMap) && charHexMap[key & 0x7F] != 0) || (int) key == EOF) {
		_buffer[getBufferIndex(_bufferSize)] = key;
		_bufferSize++;
		semPost("keyboard");
	}
}

int8_t getScancode() {
	if (_bufferSize > 0) {
		int8_t c = _buffer[getBufferIndex(0)];
		_bufferStart = getBufferIndex(1);
		_bufferSize--;
		return c;
	}
	return 0;
}

int8_t getAscii() {
	semWait("keyboard");
	int scanCode = getScancode();
	if (scanCode == EOF)
		return EOF;
	if (SHIFTED & scanCode) {
		scanCode &= 0x7F;
		return shiftedCharHexMap[scanCode];
	}
	return charHexMap[scanCode];
}
