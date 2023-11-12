#include <shellPrograms.h>
#include <stddef.h>
#include <stdint.h>
#include <syscalls.h>
#include <test_util.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

#define TEST_PIPE       100
#define TEST_BUFFER_LEN 100
#define READ            0
#define WRITE           1

static void testWriter(int argc, char **argv);
static void testReader(int argc, char **argv);

static void testWriter(int argc, char **argv) {
	openPipe(TEST_PIPE, WRITE, getPid());
	char *msg = "Prueba";
	printf("Escribiendo mensaje: %s\n", msg);
	int len = 7;
	for (size_t i = 0; i < len; i++) {
		write(TEST_PIPE, *(msg + i));
	}
	printf("Mensaje escrito\n");
	closePipe(TEST_PIPE, getPid());
}

static void testReader(int argc, char **argv) {
	printf("Esperando mensaje...\n");
	openPipe(TEST_PIPE, READ, getPid());
	char dst[TEST_BUFFER_LEN] = {0};
	read(TEST_PIPE, dst, TEST_BUFFER_LEN);
	printf("Leyendo mensaje: %s\n", dst);
	closePipe(TEST_PIPE, getPid());
}

int testNamedPipes(int argc, char **argv) {
	if (argc != 1) {
		printErr("testNamedPipes: Invalid number of arguments\n");
		return -1;
	}
	char *paramsReader[] = {"test_reader", NULL};
	char *paramsWriter[] = {"test_writer", NULL};
	uint16_t pidReader = createProcess((ProcessCode) testReader, paramsReader, "test_reader", 1);
	uint16_t pidWriter = createProcess((ProcessCode) testWriter, paramsWriter, "test_writer", 1);
	waitpid(pidReader);
	waitpid(pidWriter);
	return 0;
}