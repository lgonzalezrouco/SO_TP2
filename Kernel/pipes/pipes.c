#include <lib.h>
#include <memoryManager.h>
#include <pipes.h>
#include <processes.h>
#include <scheduler.h>
#include <stdbool.h>

#define MAX_PIPES 512

typedef struct Pipe {
	int16_t inPid;
	int16_t outPid;
	uint16_t readPosition;
	uint16_t writePosition;
	uint16_t charRemaining;
	char buffer[PIPE_SIZE];
	bool isBlocking;
} Pipe;

static void blockPipe(Pipe *pipe, pipeModes mode);
static void unblockPipe(Pipe *pipe, pipeModes mode);
static Pipe *createPipe();
static int16_t getIndex(uint16_t id);
static Pipe *getPipe(uint16_t id);
static void freePipe(uint16_t id);

Pipe *pipes[MAX_PIPES];

void initializePipes() {
	for (int i = 0; i < MAX_PIPES; i++)
		pipes[i] = NULL;
}

int8_t openPipe(uint16_t id, pipeModes mode, uint16_t pid) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return NOT_FOUND;

	Pipe *pipe = pipes[index];

	if (pipe == NULL) {
		pipe = createPipe();
		if (pipe == NULL)
			return PIPE_ERROR;
		pipes[index] = pipe;
	}

	if (mode == READ && pipe->outPid < 0)
		pipe->outPid = pid;
	else if (mode == WRITE && pipe->inPid < 0)
		pipe->inPid = pid;
	else
		return PIPE_ERROR;

	return SUCCESS;
}

int8_t closePipe(uint16_t id, uint16_t pid) {
	Pipe *pipe = getPipe(id);
	if (pipe == NULL || (pipe->inPid != pid && pipe->outPid != pid))
		return PIPE_ERROR;

	if (pipe->inPid == pid)
		pipe->inPid = DEV_NULL;
	else{
		pipe->outPid = DEV_NULL;
		freePipe(id);
	}

	return SUCCESS;
}

int64_t writePipe(uint16_t id, char *src, uint64_t len, uint16_t pid) {
	Pipe *pipe = getPipe(id);

	if (pipe == NULL || pipe->inPid != pid || len == 0)
		return PIPE_ERROR;

	uint64_t qtyWritten = 0;
	while (qtyWritten < len && pipe->buffer[pipe->writePosition] != EOF) {
		if (pipe->charRemaining == PIPE_SIZE)
			blockPipe(pipe, WRITE);

		if (pipe != getPipe(id))  // El proceso que lee del pipe puede haberlo cerrado en caso de haber finalizado
			return PIPE_ERROR;

		while (pipe->charRemaining < PIPE_SIZE && qtyWritten < len) {
			pipe->buffer[pipe->writePosition] = src[qtyWritten];
			if (src[qtyWritten++] == EOF)
				break;

			pipe->writePosition = (pipe->writePosition + 1) % PIPE_SIZE;
			pipe->charRemaining++;
		}

		if (pipe->isBlocking)
			unblockPipe(pipe, READ);
	}
	return qtyWritten;
}

int64_t readPipe(uint16_t id, char *dst, uint64_t len, uint16_t pid) {
	Pipe *pipe = getPipe(id);
	if (pipe == NULL || pipe->outPid != pid || len == 0 /* || pipe->buffer[pipe->readPosition] == EOF */)
		return PIPE_ERROR;

	uint64_t qtyRead = 0;
	while (qtyRead < len && pipe->buffer[pipe->readPosition] != EOF) {
		if (pipe->charRemaining == 0)
			blockPipe(pipe, READ);

		while ((pipe->charRemaining > 0 || pipe->buffer[pipe->readPosition] == EOF) && qtyRead < len) {
			dst[qtyRead] = pipe->buffer[pipe->readPosition];
			if (dst[qtyRead++] == EOF)
				break;

			pipe->readPosition = (pipe->readPosition + 1) % PIPE_SIZE;
			pipe->charRemaining--;
		}

		if (pipe->isBlocking)
			unblockPipe(pipe, WRITE);
	}
	return qtyRead;
}

static void blockPipe(Pipe *pipe, pipeModes mode) {
	if (mode == READ)
		blockProcess((uint16_t) pipe->outPid);
	else
		blockProcess((uint16_t) pipe->inPid);

	pipe->isBlocking = true;
	yield();
}

static void unblockPipe(Pipe *pipe, pipeModes mode) {
	if (mode == READ)
		unblockProcess((uint16_t) pipe->outPid);
	else
		unblockProcess((uint16_t) pipe->inPid);

	pipe->isBlocking = false;
}

static Pipe *createPipe() {
	Pipe *pipe = (Pipe *) malloc(sizeof(Pipe));
	if (pipe == NULL)
		return NULL;
	pipe->inPid = DEV_NULL;
	pipe->outPid = DEV_NULL;
	pipe->writePosition = 0;
	pipe->readPosition = 0;
	pipe->charRemaining = 0;
	pipe->isBlocking = false;
	memset(pipe->buffer, 0, PIPE_SIZE);
	return pipe;
}

static int16_t getIndex(uint16_t id) {
	if (id - STD_FDS < 0 || id - STD_FDS > MAX_PIPES)
		return NOT_FOUND;
	return id - STD_FDS;
}

static Pipe *getPipe(uint16_t id) {
	int16_t index;
	if ((index = getIndex(id)) == NOT_FOUND)
		return NULL;
	return pipes[index];
}

static void freePipe(uint16_t id){
	Pipe *pipe = getPipe(id);
	if(pipe == NULL)
		return;
	free(pipe);
	pipes[getIndex(id)] = NULL;
}