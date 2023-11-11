#include <test_pipes.h>

static void testWriter(int argc, char **argv);
static void testReader(int argc, char **argv);

static void testWriter(int argc, char **argv) {
	openPipe(TEST_PIPE, WRITE, getPid());
	char *msg = "Prueba";
	printf("Escribiendo mensaje: %s\n", msg);
	int len = 7;
	for (size_t i = 0; i < len; i++)
	{
		write(TEST_PIPE, *(msg + i));
	}
	printf("Mensaje escrito\n");
	closePipe(TEST_PIPE, getPid());
}

static void testReader(int argc, char **argv) {
	printf("Esperando mensaje...\n");
	openPipe(TEST_PIPE, READ, getPid());
	char dst[TEST_BUFFER_LEN] = {0};
	read(TEST_PIPE, dst, 7);
	printf("Leyendo mensaje: %s\n", dst);
	closePipe(TEST_PIPE, getPid());
}

int testNamedPipes() {
	char *paramsWriter[] = {"test_writer", NULL};
	uint16_t pidWriter = createProcess(1, (void *) &testWriter, paramsWriter, "test_writer", 4);
	char *paramsReader[] = {"test_reader", NULL};
	uint16_t pidReader = createProcess(1, (void *) &testReader, paramsReader, "test_reader", 4);
	waitpid(pidWriter);
	waitpid(pidReader);
	return 0;
}