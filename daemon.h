/**
* Tiago Henrique Pires Pinto (Trabalhador-Estudante), n.º 2006124596
* Used about 15 hours on this assignment.
**/

#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

void exit_program(int sig);
void clean_fifo();