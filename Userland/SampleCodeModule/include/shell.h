#ifndef _SHELL_H
#define _SHELL_H
#define QTY_COMMANDS 14
#define INVALID_PROCESS -1 // todo ver si estos define los dejamos aca o los ponemos en otro lado
#define NOT_FOUND -2
#define INVALID_PRIORITY -3
#define SAME_PRIORITY -4
/**
 * @brief Corre la terminal
 *
 */
void run_shell();
#endif