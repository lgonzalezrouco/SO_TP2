#include <shellPrograms.h>
#include <syscalls.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <test_util.h>

#define DEFAULT_SECONDS 2

int loop(int argc, char **argv){
    if(argc > 2){
        printErr("loop: Numero invalido de argumentos.\n");
        return -1;
    }
    int seconds = (argc == 1) ? DEFAULT_SECONDS : satoi(argv[1]);
    if(seconds <= 0){
        printErr("loop: Los segundos no pueden ser negativos ni 0.\n");
        return -1;
    }
    int16_t pid = getPid();
    while(1){
        //busyWait(75000000); // todo cambiar a sleep
        sleep(seconds);
        printf("Hola! Soy el proceso %d y estoy en un loop infinito.\n", pid);
    }
    return 0;
}