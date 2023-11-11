#ifndef _TEST_PRIO
#define _TEST_PRIO

#include "syscalls.h"
#include "test_util.h"
#include "types.h"

#include <stdint.h>
#include <stdio.h>

#define MINOR_WAIT "500000000"  // TODO: Change this value to prevent a process from flooding the screen
#define WAIT \
	1000000000  // TODO: Change this value to make the wait long enough to see theese processes beeing run at least
	            // twice

#define TOTAL_PROCESSES 3
#define LOWEST          1  // TODO: Change as required
#define MEDIUM          3  // TODO: Change as required
#define HIGHEST         5  // TODO: Change as required

/**
 * @brief Testea la funcionalidad de prioridades
 */
void test_prio();

#endif