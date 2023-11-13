#include <interrupts.h>
#include <stdint.h>
#include <time.h>

static uint64_t ticks = 0;

void timerHandler() {
	ticks++;
}

uint64_t ticksElapsed() {
	return ticks;
}

int secondsElapsed() {
	return ticks / 18;
}

uint32_t getSeconds() {
	uint8_t h, m, s;
	getTime(&h, &m, &s);
	return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

void sleep(int seconds) {
	int currentTicks = ticks;
	while (ticks - currentTicks < seconds * 18)
		_hlt();
}