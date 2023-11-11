#ifndef _TEST_PIPES_H
#define _TEST_PIPES_H

#include "shell.h"
#include "syscalls.h"
#include "test_util.h"
#include "types.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_PIPE       100
#define TEST_BUFFER_LEN 100
#define READ            0
#define WRITE           1

int testNamedPipes(int argc, char **argv);

#endif