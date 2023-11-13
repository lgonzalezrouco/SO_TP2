#include <stdint.h>

#define SEM_ID               "sem"
#define TOTAL_PAIR_PROCESSES 2

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void busyWait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);