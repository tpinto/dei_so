/**
* Tiago Henrique Pires Pinto (Trabalhador-Estudante), n.º 2006124596
* Used about 15 hours on this assignment.
**/

#include "daemon.h"
#include "globals.h"

int res_forked;
int con_forked;
int mon_forked;

int clients_pipe;

int res_to_parent_pipe[2];
int con_to_parent_pipe[2];
int mon_to_parent_pipe[2];

int parent_to_res_pipe[2];
int parent_to_con_pipe[2];
int parent_to_mon_pipe[2];

FILE * log_file;

int main(int argc, char *argv[])
{
	(void) signal(SIGINT, exit_program);
	
	int status;
	
	pipe(res_to_parent_pipe);
	pipe(con_to_parent_pipe);
	pipe(mon_to_parent_pipe);
	pipe(parent_to_res_pipe);
	pipe(parent_to_con_pipe);
	pipe(parent_to_mon_pipe);
	
	log_file = fopen("output.log", "a");
	if (log_file == NULL){
		perror("ERR");
		return 1;
	}
	
	res_forked = fork();
	if(res_forked == 0){		
		close(res_to_parent_pipe[0]);
		close(parent_to_res_pipe[1]);
		
		for(;;){
				int maxfd = 0;
				fd_set readset;
				FD_ZERO(&readset);

				FD_SET(parent_to_res_pipe[0], &readset);
				maxfd = parent_to_res_pipe[0]+1;

				int nfds = select(maxfd, &readset, NULL, NULL, NULL);

			if(nfds == -1){
				perror("ERR");
				return -1;
			}else if(FD_ISSET(parent_to_res_pipe[0], &readset)){
				//printf("CON WORKER RECEIVED STUFF\n");

				int numread;
				char mystuff[MAX_PIPE_BUF_SIZE];

				numread = read(parent_to_res_pipe[0], mystuff, MAX_PIPE_BUF_SIZE);
				if(numread  == -1){
					perror("ERR");
					return 1;
				}else{
					int pid;
					char option[1];
					char filein[MAX_PIPE_BUF_SIZE];
					char fileout[MAX_PIPE_BUF_SIZE];
					
					mystuff[numread] = '\0';

					sscanf(mystuff, "%d %c '%[^']' '%[^']'", &pid, option, filein, fileout);

					//printf("Asked to convert %s to 320x240.\n", filein);

					int child = fork();
					if(child == 0){
						close(STDOUT_FILENO);
						close(STDERR_FILENO);
						//printf("Going to convert %s to 320x240. Output file: %s.\n", filein, fileout);
						execlp("convert", "convert", "-resize", "320x240", filein, fileout, NULL);
						perror("Error running convert");
						return 1;
					}
					
					int status;
					int cena;
					while((cena = waitpid(child, &status, WUNTRACED)) != -1){
						char msg[50];
						
						if(status > 0){
							sprintf(msg, "%d ERR '%s' '%s'", pid, filein, fileout);
							write(res_to_parent_pipe[1], msg, strlen(msg));
						}else if(status == 0){
							sprintf(msg, "%d OK '%s' '%s'", pid, filein, fileout);
							write(res_to_parent_pipe[1], msg, strlen(msg));
						}
						//printf("status: %d, cena: %d, pid: %d\n", status, cena, pid);
					}

				}
			}
		}
		
		
		//sleep(5);
		exit(1);
	}else{
		mon_forked = fork();
		if(mon_forked == 0){
			close(mon_to_parent_pipe[0]);
			close(parent_to_mon_pipe[1]);
			
			for(;;){
					int maxfd = 0;
					fd_set readset;
					FD_ZERO(&readset);

					FD_SET(parent_to_mon_pipe[0], &readset);
					maxfd = parent_to_mon_pipe[0]+1;

					int nfds = select(maxfd, &readset, NULL, NULL, NULL);

				if(nfds == -1){
					perror("ERR");
					return -1;
				}else if(FD_ISSET(parent_to_mon_pipe[0], &readset)){
					//printf("CON WORKER RECEIVED STUFF\n");
					
					int numread;
					char mystuff[MAX_PIPE_BUF_SIZE];
				
					numread = read(parent_to_mon_pipe[0], mystuff, MAX_PIPE_BUF_SIZE);
					if(numread  == -1){
						perror("ERR");
						return 1;
					}else{
						int pid;
						char option[1];
						char filein[MAX_PIPE_BUF_SIZE];
						char fileout[MAX_PIPE_BUF_SIZE];
						
						mystuff[numread] = '\0';

						sscanf(mystuff, "%d %c '%[^']' '%[^']'", &pid, option, filein, fileout);
						
						//printf("Asked to convert %s to monochrome.\n", filein);
						
						int child = fork();
						if(child == 0){
							close(STDOUT_FILENO);
							close(STDERR_FILENO);
							//printf("Going to convert %s to monochrome. Output file: %s.\n", filein, fileout);
							execlp("convert", "convert", "-monochrome", filein, fileout, NULL);
							exit(1);
						} //else{
						
						int status;
						int cena;
						while((cena = waitpid(child, &status, WUNTRACED)) != -1){
							char msg[50];
							
							if(status > 0){
								sprintf(msg, "%d ERR '%s' '%s'", pid, filein, fileout);
								write(mon_to_parent_pipe[1], msg, strlen(msg));
							}else if(status == 0){
								sprintf(msg, "%d OK '%s' '%s'", pid, filein, fileout);
								write(mon_to_parent_pipe[1], msg, strlen(msg));
							}
							//printf("status: %d, cena: %d, pid: %d\n", status, cena, pid);
						}
					}
				}
			}
			
			//sleep(10);
			exit(1);
		}else{
			con_forked = fork();
			if(con_forked == 0){
				close(con_to_parent_pipe[0]);
				close(parent_to_con_pipe[1]);
				
				for(;;){
						int maxfd = 0;
						fd_set readset;
						FD_ZERO(&readset);

						FD_SET(parent_to_con_pipe[0], &readset);
						maxfd = parent_to_con_pipe[0]+1;

						int nfds = select(maxfd, &readset, NULL, NULL, NULL);

					if(nfds == -1){
						perror("ERR");
						return -1;
					}else if(FD_ISSET(parent_to_con_pipe[0], &readset)){
						//printf("CON WORKER RECEIVED STUFF\n");

						int numread;
						char mystuff[MAX_PIPE_BUF_SIZE];

						numread = read(parent_to_con_pipe[0], mystuff, MAX_PIPE_BUF_SIZE);
						if(numread  == -1){
							perror("ERR");
							return 1;
						}else{
							int pid;
							char option[1];
							char filein[MAX_PIPE_BUF_SIZE];
							char fileout[MAX_PIPE_BUF_SIZE];
							
							mystuff[numread] = '\0';

							sscanf(mystuff, "%d %c '%[^']' '%[^']'", &pid, option, filein, fileout);

							//printf("Asked to convert %s to another file type (%s).\n", filein, fileout);
							
							int child = fork();
							if(child == 0){
								close(STDOUT_FILENO);
								close(STDERR_FILENO);
								//printf("Going to convert %s. Output file: %s.\n", filein, fileout);
								execlp("convert", "convert", filein, fileout, NULL);
								perror("Error running convert");
								return 1;
							}
							
							int status;
							int cena;
							while((cena = waitpid(child, &status, WUNTRACED)) != -1){
								char msg[50];

								if(status > 0){
									sprintf(msg, "%d ERR '%s' '%s'", pid, filein, fileout);
									write(con_to_parent_pipe[1], msg, strlen(msg));
								}else if(status == 0){
									sprintf(msg, "%d OK '%s' '%s'", pid, filein, fileout);
									write(con_to_parent_pipe[1], msg, strlen(msg));
								}
								//printf("status: %d, cena: %d, pid: %d\n", status, cena, pid);
							}

						}
					}
				}

				//sleep(15);
				exit(1);
			}else{
				close(mon_to_parent_pipe[1]);
				close(parent_to_mon_pipe[0]);
				close(res_to_parent_pipe[1]);
				close(parent_to_res_pipe[0]);
				close(con_to_parent_pipe[1]);
				close(parent_to_con_pipe[0]);

				//printf("Looking for existing pipe.\n");

				int file = open(NAMED_PIPE, O_RDONLY, PIPE_SPERM);

				if(file != -1){
					//printf("Pipe exists. ");
					close(file);
					if(unlink(NAMED_PIPE) != -1){
						//printf("Deleted.\n");
					}else{
						perror("Error deleting file");
						return 1;
					}
				}else{
					//printf("Doesn't exist.\n");
				}

				if((mkfifo(NAMED_PIPE, PIPE_SPERM) == -1))
				{
					perror("Failed to create the pipe");
					return 1;
				}
				/*else{
					printf("[%ld] Created named pipe '%s'.\n",(long)getpid(),NAMED_PIPE);
				}*/

				while(((clients_pipe = open(NAMED_PIPE, PIPE_OFLAG, PIPE_SPERM)) == -1) && (errno == EINTR));
				if(clients_pipe == -1){
					perror("Failed to open named pipe");
					return 1;
				}
				
				
				//printf("starting to handle requests\n");
				
				for(;;){
					int maxfd = 0;
					fd_set readset;
					FD_ZERO(&readset);
					
					FD_SET(clients_pipe, &readset);
					
					maxfd = clients_pipe+1;
						
					FD_SET(con_to_parent_pipe[0], &readset);
					
					if(con_to_parent_pipe[0] >= maxfd)
						maxfd = con_to_parent_pipe[0]+1;
					
					FD_SET(mon_to_parent_pipe[0], &readset);
					
					if(mon_to_parent_pipe[0] >= maxfd)
						maxfd = mon_to_parent_pipe[0]+1;
					
					FD_SET(res_to_parent_pipe[0], &readset);
					
					if(res_to_parent_pipe[0] >= maxfd)
						maxfd = res_to_parent_pipe[0]+1;
					
					int nfds = select(maxfd, &readset, NULL, NULL, NULL);
					
					int numread;
					char stuff[MAX_PIPE_BUF_SIZE];
				
				if(nfds == -1){
					perror("ERR");
					return -1;
				}else if(FD_ISSET(con_to_parent_pipe[0], &readset)){
					char msg[MAX_PIPE_BUF_SIZE];
					
					numread = read(con_to_parent_pipe[0], msg, MAX_PIPE_BUF_SIZE);
					
					msg[numread] = '\0';
					
					int lpid;
					char status[5];
					char filein[255];
					char fileout[255];

					sscanf(msg, "%d %s '%[^']' '%[^']'", &lpid, status, filein, fileout);
					
					if(strcmp(status, "OK") == 0){
						fprintf(log_file, "%s\t%s\tConvert\tSuccess\n", filein, fileout);
						kill(lpid, SIGUSR1);
					}else if(strcmp(status, "ERR") == 0){
						fprintf(log_file, "%s\t%s\tConvert\tError\n", filein, fileout);
						kill(lpid, SIGUSR2);
					}else{
						printf("ERR");
					}
					
					//printf("RECEIVED: \"%.*s\"\n", numread, stuff);
				}else if(FD_ISSET(mon_to_parent_pipe[0], &readset)){
					char msg[MAX_PIPE_BUF_SIZE];
					
					numread = read(mon_to_parent_pipe[0], msg, MAX_PIPE_BUF_SIZE);
					
					msg[numread] = '\0';
					
					int lpid;
					char status[5];
					char filein[255];
					char fileout[255];

					sscanf(msg, "%d %s '%[^']' '%[^']'", &lpid, status, filein, fileout);
					
					if(strcmp(status, "OK") == 0){
						fprintf(log_file, "%s\t%s\tB&W\tSuccess\n", filein, fileout);
						kill(lpid, SIGUSR1);
					}else if(strcmp(status, "ERR") == 0){
						fprintf(log_file, "%s\t%s\tB&W\tError\n", filein, fileout);
						kill(lpid, SIGUSR2);
					}else{
						printf("ERR");
					}
					
					//printf("RECEIVED: \"%.*s\"\n", numread, msg);
				}else if(FD_ISSET(res_to_parent_pipe[0], &readset)){
						char msg[MAX_PIPE_BUF_SIZE];

						numread = read(res_to_parent_pipe[0], msg, MAX_PIPE_BUF_SIZE);

						msg[numread] = '\0';

						int lpid;
						char status[5];
						char filein[255];
						char fileout[255];

						sscanf(msg, "%d %s '%[^']' '%[^']'", &lpid, status, filein, fileout);

						if(strcmp(status, "OK") == 0){
							fprintf(log_file, "%s\t%s\tResize\tSuccess\n", filein, fileout);
							kill(lpid, SIGUSR1);
						}else if(strcmp(status, "ERR") == 0){
							fprintf(log_file, "%s\t%s\tResize\tError\n", filein, fileout);
							kill(lpid, SIGUSR2);
						}else{
							printf("ERR");
						}

						//printf("RECEIVED: \"%.*s\"\n", numread, msg);
					}else if(FD_ISSET(clients_pipe, &readset)){
					
					numread = read(clients_pipe, stuff, MAX_PIPE_BUF_SIZE);
					
					//printf("RECEIVED: \"%.*s\"\n", numread, stuff);

					int pid;
					char option[1];
					char filein[MAX_PIPE_BUF_SIZE];
					char fileout[MAX_PIPE_BUF_SIZE];

					stuff[numread] = '\0';

					sscanf(stuff, "%d %c '%[^']' '%[^']'", &pid, option, filein, fileout);
					
					switch(option[0]){
						case 'c':
							//printf("%d asked to convert %s to jpeg (%s)\n", pid, filein, fileout);
							write(parent_to_con_pipe[1], stuff, strlen(stuff));
							break;
						case 'r':
							//printf("%d asked to convert %s resolution to 320x240\n", pid, filein);
							write(parent_to_res_pipe[1], stuff, strlen(stuff));
							break;
						case 'm':
							//printf("%d asked to convert %s to monochrome (%s)\n", pid, filein, fileout);
							write(parent_to_mon_pipe[1], stuff, strlen(stuff));
							break;
						default:
							//printf("%d sent option '%c' -- there's no such thing!\n", pid, option[0]);
							break;
					}
					
				}else{
					printf("ERR");
				}
				fflush(log_file);
			}
				
				while(wait(&status) > 0);
				exit(0);
			}
		}
	}

	clean_fifo();
	return 0;
}


void exit_program(int sig) {
	clean_fifo();
	kill(con_forked, SIGINT);
	kill(mon_forked, SIGINT);
	kill(res_forked, SIGINT);
	(void) signal(SIGINT, SIG_DFL);
}

void clean_fifo(){
	close(clients_pipe);
	close(res_to_parent_pipe[0]);
	close(con_to_parent_pipe[0]);
	close(mon_to_parent_pipe[0]);
	close(parent_to_res_pipe[0]);
	close(parent_to_con_pipe[0]);
	close(parent_to_mon_pipe[0]);
	close(res_to_parent_pipe[1]);
	close(con_to_parent_pipe[1]);
	close(mon_to_parent_pipe[1]);
	close(parent_to_res_pipe[1]);
	close(parent_to_con_pipe[1]);
	close(parent_to_mon_pipe[1]);
	fclose(log_file);
	unlink(NAMED_PIPE);
}