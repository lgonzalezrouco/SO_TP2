#include <syscalls.h>
#include <test_util.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

#define MAX_PHILOS   20
#define START_PHILOS 5

#define ADD_PHILOSOPHER    'a'
#define REMOVE_PHILOSOPHER 'r'
#define QUIT_PHILOSOPHERS  'q'

#define THINKING_TIME 10000000

#define MUTEX "mutex"

#define philosopherSemaphore(i) ("philosopherSemaphore" + (i))
#define left(i) ((i + qtyPhilos - 1) % qtyPhilos)
#define right(i) ((i + 1) % qtyPhilos)

typedef enum { NOT_SET, EATING, HUNGRY, THINKING } State;

State state[MAX_PHILOS];
int qtyPhilos = START_PHILOS;
int pidPhilos[MAX_PHILOS];

static const char *philoNames[] = {"Aristotle", "Kant",      "Spinoza",      "Marx",      "Russell",
                                   "Plato",     "Descartes", "Wittgenstein", "Locke",     "Kierkegaard",
                                   "Hume",      "Sartre",    "Aquinas",      "Confucius", "Epicurus",
                                   "Heidegger", "Hegel",     "Hobbes",       "Nietzsche", "Socrates"};

static int addPhilosopher(int philoNum);
static int removePhilosopher();
static int philosopher(int argc, char *argv[]);
static void test(int phnum);
static void takeFork(int phnum);
static void putFork(int phnum);

int philo(int argc, char **argv) {
	if (semInit(MUTEX, 1) == -1) {
		printf("Error creating mutex\n");
		return;
	}
	int i = 0;
	for (i = 0; i < MAX_PHILOS; i++) {
		state[i] = NOT_SET;
		if (i < START_PHILOS)
			addPhilosopher(qtyPhilos);
	}

	char c = 0;
	while (((c = getchar()) != QUIT_PHILOSOPHERS)) {
		if (c == ADD_PHILOSOPHER) {
			addPhilosopher(qtyPhilos);
		} else if (c == REMOVE_PHILOSOPHER) {
			removePhilosopher(qtyPhilos);
		}
	}
}

static int addPhilosopher(int philoNum) {
	if (qtyPhilos >= MAX_PHILOS) {
		printf("Max philosophers reached\n");
		return -1;
	}

	semWait(MUTEX);
	if (semInit(philosopherSemaphore(qtyPhilos), 0) == -1) {
		printf("Error creating philosopher semaphore\n");
		return -1;
	}
    char* phNumStr;
    intToString(philoNum, phNumStr);
    char *args[] = {philoNames[philoNum], phNumStr, NULL};
	int fdsPhilo[] = {DEV_NULL, STDOUT, STDERR};
    int pid = createProcessFds((ProcessCode) &philosopher, args, "philosopher", 0, fdsPhilo); // todo esta bien que sea background?
    if (pid == -1) {
        printf("Error creating philosopher process\n");
        return -1;
    }
    qtyPhilos++;
	semPost(MUTEX);
}

static int removePhilosopher(int philoNum) {
    if (qtyPhilos <= 0) {
        printf("No philosophers to remove\n");
        return -1;
    }

    semWait(MUTEX);
    
    killProcess(pidPhilos[qtyPhilos - 1]);
    waitpid(pidPhilos[qtyPhilos - 1]);

    semPost(MUTEX);
}

static int philosopher(int argc, char *argv[]) {
    int phnum = atoi(argv[1]);
    state[phnum] = THINKING;
    while (1) {
        busyWait(THINKING_TIME);

        takeFork(phnum);

        putFork(phnum);
    }
}

static void test(int phnum) {
	if (state[phnum] == HUNGRY && state[left(phnum)] != EATING && state[right(phnum)] != EATING) {
		// state that eating
		state[phnum] = EATING;

		busyWait(THINKING_TIME);

		printf("Philosopher %d takes fork %d and %d\n", phnum + 1, left(phnum) + 1, phnum + 1);

		printf("Philosopher %d is Eating\n", phnum + 1);

		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		semPost(philosopherSemaphore(phnum));
	}
}

static void takeFork(int phnum) {
	semWait(MUTEX);
    state[phnum] = HUNGRY;

	test(phnum);

	semPost(MUTEX);
	semWait(philosopherSemaphore(phnum));
}

static void putFork(int phnum) {
	semWait(MUTEX);

	state[phnum] = THINKING;

    printf("Philosopher %d putting fork %d and %d down\n", phnum + 1, left(phnum) + 1, phnum + 1);
    printf("Philosopher %d is thinking\n", phnum + 1);

    test(left(phnum));
    test(right(phnum));

	semPost(MUTEX);
}

/*
void philosopher(int argc, char **argv) {
	printf("argc: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d]: %s\n", i, argv[i]);
	}
	int8_t phnum = atoi(argv[3]);
	printf("num: aksx %d\n", phnum);
	while (1) {
		int i = atoi(argv[3]);

		busyWait(THINKING_TIME);

		take_fork(i);

		put_fork(i);
	}
}
*/

/*
int state[MAX_PHILOSOPHERS];
int phil_num[MAX_PHILOSOPHERS];

char buffer[64];
int16_t pids[MAX_PHILOSOPHERS];
int philoQty = 0;

void *mutex;
void *S[MAX_PHILOSOPHERS];
static char *getId(char *prefix, int id);
static void philospher(int argc, char *args[]);
static void takeFork(int);
static void putFork(int);
static void test(int);
static void printPhilosophers();
static void addPhilosopher();
static void removePhilosopher();

void philo() {
    philoQty = 5;
    char* args[] = {"0", NULL};
    philospher(0, args);
    int i;

    semInit("mutex", 1);

    for (i = 0; i < START_PHILOSOPHERS; i++) {
        addPhilosopher();
    }

    char c;
    while (1) {
        read(0, &c, 1);
        if (c == 'a') {
            addPhilosopher();
        } else if (c == 'r') {
            removePhilosopher();
        }
        yield();
    }

    for (i = 0; i < philoQty; i++)
        waitpid(pids[i]);
}

static char *getId(char *prefix, int id) {
    int n = 0;
    n += strcpy(buffer, prefix);
    intToString(id, buffer + n);
    return buffer;
}

static void philospher(int argc, char *args[]) {
    int number = atoi(args[0]);
    while (1) {
        int i = number;
        busyWait(THINKING_TIME);
        takeFork(i);
        yield();
        putFork(i);
    }
}

static void takeFork(int ph_num) {
    semWait("mutex");
    state[ph_num] = HUNGRY;

    test(ph_num);
    semPost("mutex");
    semWait(getId("sem_", ph_num));
}

static void putFork(int ph_num) {
    semWait("mutex");
    state[ph_num] = THINKING;
    printPhilosophers();
    test(LEFT);
    test(RIGHT);
    semPost("mutex");
}

static void test(int ph_num) {
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[ph_num] = EATING;
        semPost("mutex");
        busyWait(EATING_TIME);
        semWait("mutex");
        printPhilosophers();
        semPost(getId("sem_", ph_num));
    }
}

static void printPhilosophers() {
    char bufferStart[100];
    char *bufferCurrent = bufferStart;
    for (int i = 0; i < philoQty; i++) {
        printf("%d", state[i]);
        switch (state[i]) {
            case EATING:
                bufferCurrent += strcpy(bufferCurrent, " E ");
                break;
            case THINKING:
                bufferCurrent += strcpy(bufferCurrent, " . ");
                break;
            case HUNGRY:
                bufferCurrent += strcpy(bufferCurrent, " X ");
                break;
        }
    }
    printf("%s\n", bufferStart);
}

static void addPhilosopher() {
    printf("%s \n", "adding");
    printPhilosophers();
    semWait("mutex");
    char param[16] = {0};
    char *args[] = {param};
    phil_num[philoQty] = philoQty;
    semInit(getId("sem_", philoQty), 1);
    intToString((unsigned long long) &phil_num[philoQty], args[0]);
    pids[philoQty] = createProcess((int16_t) 1, (ProcessCode) &philospher, args, "philospher", (uint8_t) 5);
    unblockProcess(pids[philoQty]);
    philoQty++;
    semPost("mutex");
}

static void removePhilosopher() {
    if (philoQty <= 0)
        return;
    if (state[philoQty - 1] == EATING)
        putFork(philoQty - 1);
    semWait("mutex");
    printf("%s \n", "removing");
    printPhilosophers();
    philoQty--;
    semClose(getId("sem_", philoQty));
    killProcess(pids[philoQty]);
    semPost("mutex");
}
*/