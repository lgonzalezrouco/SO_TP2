#ifndef _PIPES_H
#define _PIPES_H

#include <stdint.h>
#include <types.h>

#define PIPE_SIZE 512

int8_t initPipes();
int8_t openPipe(uint16_t id, uint8_t mode, uint16_t pid);
int8_t closePipe(uint16_t id, uint8_t mode);
int64_t readPipe(uint16_t id, char *dst, uint64_t len, uint16_t pid);
int64_t writePipe(uint16_t id, char *src, uint64_t len, uint16_t pid);

#endif