#ifndef _TEST_SYNC
#define _TEST_SYNC

#include "syscalls.h"
#include "test_util.h"
#include "types.h"

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Testea la sincronizacion entre procesos
 * @param n: Cantidad de sumas y restas que hace, debe ser mayor a 0
 * @param use_sem: Si es 1 usa semaforos, si es 0 no usa semaforos
 */
uint64_t test_sync(char * n, char * use_sem);

#endif