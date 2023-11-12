#include <syscalls.h>
#include <test_util.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

//#define THINKING           0
//#define HUNGRY             1
//#define EATING             2
//#define LEFT               (ph_num + (phyloQty - 1)) % phyloQty
//#define RIGHT              (ph_num + 1) % phyloQty
#define THINKING_TIME      75000000
#define EATING_TIME        50000000
#define MAX_PHILOSOPHERS   100
#define START_PHILOSOPHERS 5

/**
 * @brief      Starts the philosophers problem
 */
void phylo();