#include <defs.h>
#include <memoryManager.h>
#include <pipes.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

#define MAX_PIPES           512
#define bufferPositio(pipe) (((pipe)->startPosition + (pipe)->currentSize) % PIPE_SIZE)

typedef struct Pipe {
	int16_t inPid;
	int16_t outPid;
	char buffer[PIPE_SIZE];
	uint16_t startPosition;
	uint16_t currentSize;
	uint8_t isBlocking;
} Pipe;

static int16_t getIndex(uint16_t id);
static Pipe *getPipe(uint16_t id);
static Pipe *createPipe();
static void block(Pipe *pipe, uint8_t mode);
static void unblock(Pipe *pipe, uint8_t mode);

Pipe *pipes[MAX_PIPES];

int8_t initPipes() {
	for (int i = 0; i < MAX_PIPES; i++)
		pipes[i] = NULL;
	return 0;
}

int8_t openPipe(uint16_t id, uint8_t mode, uint16_t pid) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return NOT_FOUND;
	Pipe *pipe = pipes[index];
	if (pipe == NULL) {
		pipe = createPipe();
		pipes[index] = pipe;
	}
	if (mode == READ && pipe->outPid < 0) {
		pipe->outPid = pid;
	} else if (mode == WRITE && pipe->inPid < 0) {
		pipe->inPid = pid;
	} else {
		return PROBLEM;
	}
	return SUCCESS;
}

int8_t closePipe(uint16_t id, uint8_t mode) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return NOT_FOUND;
	Pipe *pipe = pipes[index];
	if (pipe == NULL)
		return PROBLEM;
	if (mode == READ && pipe->outPid >= 0)
		pipe->outPid = -1;
	else if (mode == WRITE && pipe->inPid >= 0)
		pipe->inPid = -1;
	else
		return PROBLEM;
	if (pipe->inPid < 0 && pipe->outPid < 0) {
		free(pipe);
		pipes[index] = NULL;
	}
	return SUCCESS;
}

int64_t writePipe(uint16_t id, char *src, uint64_t len, uint16_t pid) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return PROBLEM;
	Pipe *pipe = getPipe(id);
	if (pipe == NULL || pipe->inPid != pid ||
	    pipe->buffer[((pipe->startPosition + pipe->currentSize) % PIPE_SIZE)] == EOF || len == 0)
		return PROBLEM;

	int qtyWritten = 0;
	while (qtyWritten < len && src[qtyWritten] != EOF) {
		if (pipe->currentSize == PIPE_SIZE) {
			block(pipe, WRITE);
		}
		if (pipe != getPipe(id))
			return PROBLEM;

		while (pipe->currentSize < PIPE_SIZE && qtyWritten < len) {
			pipe->buffer[((pipe->startPosition + pipe->currentSize) % PIPE_SIZE)] = src[qtyWritten];
			pipe->currentSize++;
			if (src[qtyWritten++] == EOF)
				break;
		}
		if (pipe->isBlocking) {
			unblock(pipe, WRITE);
		}
	}
	return qtyWritten;
}

int64_t readPipe(uint16_t id, char *dst, uint64_t len, uint16_t pid) {
	Pipe *pipe = getPipe(id);
	if (pipe == NULL || pipe->outPid != pid || len == 0 || pipe->buffer[pipe->startPosition] == EOF)
		return PROBLEM;

	uint64_t qtyRead = 0;
	while (qtyRead < len && dst[qtyRead] != EOF) {
		if (pipe->currentSize == 0) {
			block(pipe, READ);
		}
		while (pipe->currentSize > 0 && qtyRead < len) {
			dst[qtyRead] = pipe->buffer[pipe->startPosition];
			if (dst[qtyRead++] != EOF) {
				pipe->currentSize--;
				pipe->startPosition = (pipe->startPosition + 1) % PIPE_SIZE;
			} else
				break;
		}
		if (pipe->isBlocking) {
			unblock(pipe, READ);
		}
	}
	return qtyRead;
}

static void block(Pipe *pipe, uint8_t mode) {
	if (mode == READ)
		blockProcess((uint16_t) pipe->outPid);
	else
		blockProcess((uint16_t) pipe->inPid);
	pipe->isBlocking = 1;
	yield();
}

static void unblock(Pipe *pipe, uint8_t mode) {
	if (mode == READ)
		unblockProcess((uint16_t) pipe->outPid);
	else
		unblockProcess((uint16_t) pipe->inPid);
	pipe->isBlocking = 0;
}

static Pipe *createPipe() {
	Pipe *pipe = (Pipe *) malloc(sizeof(Pipe));
	pipe->inPid = -1;
	pipe->outPid = -1;
	pipe->startPosition = 0;
	pipe->currentSize = 0;
	pipe->isBlocking = 0;
	return pipe;
}

static int16_t getIndex(uint16_t id) {
	return ((id - STD_PIPES) > MAX_PIPES || (id - STD_PIPES) < 0) ? NOT_FOUND : (id - STD_PIPES);
}

static Pipe *getPipe(uint16_t id) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return NULL;
	return pipes[index];
}