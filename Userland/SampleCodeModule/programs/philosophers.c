#include <phylosophers.h>

#define N        5
#define THINKING 2
#define HUNGRY   1
#define EATING   0
#define LEFT     (phnum + 4) % N
#define RIGHT    (phnum + 1) % N

int state[N];
int phil[N] = {0, 1, 2, 3, 4};

char *mutex = "mutex";
#define philosopherSemaphore(i) ("philosopherSemaphore" + (i))

void test(int phnum) {
	if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
		// state that eating
		state[phnum] = EATING;

		busyWait(THINKING_TIME);

		printf("Philosopher %d takes fork %d and %d\n", phnum + 1, LEFT + 1, phnum + 1);

		printf("Philosopher %d is Eating\n", phnum + 1);

		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		semPost(philosopherSemaphore(phnum));
	}
}

// take up chopsticks
void take_fork(int phnum) {
	semWait(mutex);

	// state that hungry
	state[phnum] = HUNGRY;

	printf("Philosopher %d is Hungry\n", phnum + 1);

	// eat if neighbours are not eating
	test(phnum);

	semPost(mutex);

	// if unable to eat wait to be signalled
	semWait(philosopherSemaphore(phnum));

	busyWait(THINKING_TIME);
}

// put down chopsticks
void put_fork(int phnum) {
	semWait(mutex);

	// state that thinking
	state[phnum] = THINKING;

	printf("Philosopher %d putting fork %d and %d down\n", phnum + 1, LEFT + 1, phnum + 1);
	printf("Philosopher %d is thinking\n", phnum + 1);

	test(LEFT);
	test(RIGHT);

	semPost(mutex);
}

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

void phylo() {
	int i;

	if(semInit(mutex, 1) == -1) {
        printf("Error creating mutex\n");
        return;
    }

	for (i = 0; i < N; i++){
		if(semInit(philosopherSemaphore(i), 0) == -1) {
            printf("Error creating S[%d]\n", i);
            return;
        }
    }
    
    char* philArgs[6] = {"philosopher", "1", "2", "3", "4", NULL};

	for (i = 0; i < N; i++) {
	    createProcess(1, (ProcessCode) &philosopher, philArgs, "philosopher", 5);
		printf("Philosopher %d is thinking\n", i + 1);
	}
}

/*
int state[MAX_PHILOSOPHERS];
int phil_num[MAX_PHILOSOPHERS];

char buffer[64];
int16_t pids[MAX_PHILOSOPHERS];
int phyloQty = 0;

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

void phylo() {
    phyloQty = 5;
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

    for (i = 0; i < phyloQty; i++)
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
    for (int i = 0; i < phyloQty; i++) {
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
    phil_num[phyloQty] = phyloQty;
    semInit(getId("sem_", phyloQty), 1);
    intToString((unsigned long long) &phil_num[phyloQty], args[0]);
    pids[phyloQty] = createProcess((int16_t) 1, (ProcessCode) &philospher, args, "philospher", (uint8_t) 5);
    unblockProcess(pids[phyloQty]);
    phyloQty++;
    semPost("mutex");
}

static void removePhilosopher() {
    if (phyloQty <= 0)
        return;
    if (state[phyloQty - 1] == EATING)
        putFork(phyloQty - 1);
    semWait("mutex");
    printf("%s \n", "removing");
    printPhilosophers();
    phyloQty--;
    semClose(getId("sem_", phyloQty));
    killProcess(pids[phyloQty]);
    semPost("mutex");
}
*/