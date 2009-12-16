#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

/** returns the last tweet that contains a search_term */
char *lasttweet(char *search_term);

int getlinee(char s[], int lim, FILE *fil);