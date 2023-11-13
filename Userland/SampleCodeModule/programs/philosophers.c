#include <syscalls.h>
#include <test_util.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

#define MAX_PHILOS   20
#define START_PHILOS 3

#define ADD_PHILOSOPHER    'a'
#define REMOVE_PHILOSOPHER 'r'
#define QUIT_PHILOSOPHERS  'q'

#define IS_RUNNING 1
#define NOT_RUNNING 0

#define THINKING_TIME 10000000

#define MUTEX "mutex"

#define left(i)  ((i + qtyPhilos - 1) % qtyPhilos)
#define right(i) ((i + 1) % qtyPhilos)

typedef enum { NOT_SET, EATING, HUNGRY, THINKING } State;

State state[MAX_PHILOS];
State prev[MAX_PHILOS];
int qtyPhilos = 0;
int pidPhilos[MAX_PHILOS];

static char *philoNames[] = {"Aristotle",    "Kant",      "Spinoza",     "Marx",   "Russell",   "Plato",   "Descartes",
                             "Wittgenstein", "Locke",     "Kierkegaard", "Hume",   "Sartre",    "Aquinas", "Confucius",
                             "Epicurus",     "Heidegger", "Hegel",       "Hobbes", "Nietzsche", "Socrates"};

static int addPhilosopher(int philoNum);
static int removePhilosopher();
static int philosopher(int argc, char *argv[]);
static void test(int phnum);
static void takeFork(int phnum);
static void putFork(int phnum);
static void printPhilos();

int philo(int argc, char **argv) {
	if (semInit(MUTEX, 1) == -1) {
		printf("Error creating mutex\n");
		return -1;
	}

	for (int i = 0; i < MAX_PHILOS; i++) {
		state[i] = NOT_SET;
		prev[i] = NOT_SET;
		pidPhilos[i] = -1;
		if (i < START_PHILOS)
			addPhilosopher(i);
	}

	char c = 0;
	while (((c = getchar()) != QUIT_PHILOSOPHERS)) {
		if (c == ADD_PHILOSOPHER)
			addPhilosopher(qtyPhilos);
		else if (c == REMOVE_PHILOSOPHER)
			removePhilosopher(IS_RUNNING);
	}

	for (int i = qtyPhilos - 1; i >= 0; i--)
		removePhilosopher(NOT_RUNNING);

	semClose(MUTEX);
	return 0;
}

static int addPhilosopher(int philoNum) {
	if (qtyPhilos >= MAX_PHILOS) {
		printf("No se pueden agregar mas filosofos\n");
		return -1;
	}

	semWait(MUTEX);
	if (semInit(philoNames[philoNum], 0) == -1) {
		printf("Error creating philosopher semaphore\n");
		return -1;
	}

	char phNumStr[3] = {0};
	intToString(philoNum, phNumStr);
	char *args[] = {philoNames[philoNum], phNumStr, NULL};
	pidPhilos[philoNum] = createProcess((ProcessCode) &philosopher, args, "philosopher", 0);
	if (pidPhilos[philoNum] == -1) {
		printf("Error creating philosopher process\n");
		return -1;
	}
	qtyPhilos++;
	printPhilos();
	semPost(MUTEX);
	return 0;
}

static int removePhilosopher(int running) {
	if (qtyPhilos <= START_PHILOS && running) {
		printf("No es posible eliminar filosofos\n");
		return -1;
	}

	semWait(MUTEX);

	printf("Se retira %s de la mesa\n", philoNames[qtyPhilos - 1]);

	killProcess(pidPhilos[qtyPhilos - 1]);
	waitpid(pidPhilos[qtyPhilos - 1]);

	semClose(philoNames[qtyPhilos - 1]);

	pidPhilos[qtyPhilos - 1] = -1;
	state[qtyPhilos - 1] = NOT_SET;
	qtyPhilos--;

	printPhilos();

	semPost(MUTEX);
	return 0;
}

static int philosopher(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Error a la hora de crear el proceso del filosofo\n");
		return -1;
	}

	int phnum = atoi(argv[1]);
	printf("Se une %s a la mesa\n", philoNames[phnum]);
	state[phnum] = THINKING;
	while (1) {
		sleep(2);

		takeFork(phnum);

		sleep(3);

		putFork(phnum);
	}
	return 0;
}

static void test(int phnum) {
	if (state[phnum] == HUNGRY && state[left(phnum)] != EATING && state[right(phnum)] != EATING) {
		state[phnum] = EATING;
		semPost(philoNames[phnum]);
		printPhilos();
	}
}

static void takeFork(int phnum) {
	semWait(MUTEX);
	state[phnum] = HUNGRY;

	test(phnum);

	semPost(MUTEX);
	semWait(philoNames[phnum]);
}

static void putFork(int phnum) {
	semWait(MUTEX);

	state[phnum] = THINKING;

	printPhilos();

	test(left(phnum));
	test(right(phnum));

	semPost(MUTEX);
}

static void printPhilos() {
	int sameAsPrev = 1;
	int someoneEating = 0;
	for (int i = 0; i < qtyPhilos && sameAsPrev && !someoneEating; i++) {
		if ((state[i] == EATING && prev[i] != EATING) || (state[i] != EATING && prev[i] == EATING))
			sameAsPrev = 0;
	}
	for (int i = 0; i < qtyPhilos && !sameAsPrev; i++) {
		prev[i] = state[i];
		switch (state[i]) {
			case EATING:
				printf(" E ");
				break;
			case HUNGRY:
				printf(" . ");
				break;
			case THINKING:
				printf(" . ");
				break;
			default:
				break;
		}
	}
	if (!sameAsPrev)
		printf("\n");
}