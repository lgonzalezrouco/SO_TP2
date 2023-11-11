#ifndef _PIPES_H
#define _PIPES_H

#include <stdint.h>
#include <types.h>

#define PIPE_SIZE 512

void initializePipes();
int8_t openPipe(uint16_t id, pipeModes mode, uint16_t pid);
int8_t closePipe(uint16_t id, uint16_t pid);
int64_t writePipe(uint16_t id, char *src, uint64_t len, uint16_t pid);
int64_t readPipe(uint16_t id, char *dst, uint64_t len, uint16_t pid);

#endif