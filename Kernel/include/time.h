#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>
#include <scheduler.h>

/**
 * @brief Handler del timer tick
 */
void timerHandler();
/**
 * @return Devuelve los ticks
 */
uint64_t ticksElapsed();

/**
 * @return  Devuelve los segundos
 */
int secondsElapsed();

uint32_t getSeconds();

void sleep(int seconds);

#endif
