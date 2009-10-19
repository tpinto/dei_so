/**
* Tiago Henrique Pires Pinto (Trabalhador-Estudante), n.º 2006124596
* Used about 15 hours on this assignment.
**/

#define NAMED_PIPE "REQUESTS"
#define PIPE_SPERM (S_IRWXU | S_IRWXG | S_IRWXO)
#define PIPE_OFLAG (O_RDWR | O_TRUNC | O_CREAT)
#define MAX_PIPE_BUF_SIZE 255