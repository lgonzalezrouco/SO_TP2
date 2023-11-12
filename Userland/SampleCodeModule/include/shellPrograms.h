#ifndef _SHELL_PROGRAMS_H_
#define _SHELL_PROGRAMS_H_

int time(int argc, char **argv);
int printMemInfo(int argc, char **argv);
int printProcesses(int argc, char **argv);
int kill(int argc, char **argv);
int nice(int argc, char **argv);
int toggleBlock(int argc, char **argv);
int test_mm(int argc, char **argv);
int test_processes(int argc, char **argv);
int test_sync(int argc, char **argv);
int test_prio(int argc, char **argv);
int testNamedPipes(int argc, char **argv);
int wc(int argc, char **argv);
int cat(int argc, char **argv);
int filter(int argc, char **argv);
int loop(int argc, char **argv);

#endif