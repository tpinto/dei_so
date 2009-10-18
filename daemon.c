


#include "daemon.h"
#include "globals.h"

int main(int argc, char *argv[])
{
	(void) signal(SIGINT, exit_program);

	int res_to_parent_pipe[2];
	int con_to_parent_pipe[2];
	int mon_to_parent_pipe[2];
	
	int parent_to_res_pipe[2];
	int parent_to_con_pipe[2];
	int parent_to_mon_pipe[2];
	
	int res_forked;
	int con_forked;
	int mon_forked;
	
	int status;
	
	pipe(res_to_parent_pipe);
	pipe(con_to_parent_pipe);
	pipe(mon_to_parent_pipe);
	pipe(parent_to_res_pipe);
	pipe(parent_to_con_pipe);
	pipe(parent_to_mon_pipe);
	
	res_forked = fork();
	if(res_forked == 0){
		close(res_to_parent_pipe[0]);
		close(parent_to_res_pipe[1]);
		
		int numread;
		char stuff[MAX_PIPE_BUF_SIZE];
		
		while((numread = read(parent_to_res_pipe[0], stuff, MAX_PIPE_BUF_SIZE))){
			if(numread  == -1){
				perror("Bode");
				return 1;
			}else{
				printf("Resolution, recvd: \"%.*s\"\n", numread, stuff);
			}
		}
		
		//printf("%ld - resolution\n", (long)getpid());
		//sleep(5);
		_exit(1);
	}else{
		mon_forked = fork();
		if(mon_forked == 0){
			close(mon_to_parent_pipe[0]);
			close(parent_to_mon_pipe[1]);
			
			int numread;
			char stuff[MAX_PIPE_BUF_SIZE];

			while((numread = read(parent_to_mon_pipe[0], stuff, MAX_PIPE_BUF_SIZE))){
				if(numread  == -1){
					perror("Bode");
					return 1;
				}else{
					printf("Monochrome, recvd: \"%.*s\"\n", numread, stuff);
				}
			}
			
			//printf("%ld - b&w\n", (long)getpid());
			//sleep(10);
			_exit(1);
		}else{
			con_forked = fork();
			if(con_forked == 0){
				close(con_to_parent_pipe[0]);
				close(parent_to_con_pipe[1]);
				
				int numread;
				char stuff[MAX_PIPE_BUF_SIZE];

				while((numread = read(parent_to_con_pipe[0], stuff, MAX_PIPE_BUF_SIZE))){
					if(numread  == -1){
						perror("Bode");
						return 1;
					}else{
						printf("PNG TO JPEG, recvd: \"%.*s\"\n", numread, stuff);
					}
				}
				
				//printf("%ld - to jpeg\n", (long)getpid());
				//sleep(15);
				_exit(1);
			}else{
				while(wait(&status) > 0);
				
				printf("%ld - parent\n", (long)getpid());
				
				int numread;
				char stuff[MAX_PIPE_BUF_SIZE];

				printf("Looking for existing pipe.\n");

				int file = open(NAMED_PIPE, O_RDONLY, PIPE_SPERM);

				if(file != -1){
					printf("Pipe exists. ");
					close(file);
					if(unlink(NAMED_PIPE) != -1){
						printf("Deleted.\n");
					}else{
						perror("Error deleting file");
						return 1;
					}
				}else{
					printf("Doesn't exist.\n");
				}

				if((mkfifo(NAMED_PIPE, PIPE_SPERM) == -1))
				{
					perror("Failed to create the pipe");
					return 1;
				}else{
					printf("Created named pipe '%s'.\n",NAMED_PIPE);
				}

				while(((clients_pipe = open(NAMED_PIPE, PIPE_OFLAG, PIPE_SPERM)) == -1) && (errno == EINTR));
				if(clients_pipe == -1){
					perror("Failed to open named pipe");
					return 1;
				}

				while((numread = read(clients_pipe, stuff, MAX_PIPE_BUF_SIZE))) {
					if(numread  == -1){
						perror("Bode");
						return 1;
					}else{
						printf("RECEIVED: \"%.*s\"\n", numread, stuff);

						int pid;
						char option[1];
						char filein[MAX_PIPE_BUF_SIZE];
						char fileout[MAX_PIPE_BUF_SIZE];

						sscanf(stuff, "%d -%c %s %s", &pid, option, filein, fileout);

						switch(option[0]){
							case 'c':
								printf("%d asked to convert %s to jpeg\n", pid, filein);
								write(parent_to_con_pipe[1], stuff, strlen(stuff));
								break;
							case 'r':
								printf("%d asked to convert %s resolution to 320x240\n", pid, filein);
								write(parent_to_res_pipe[1], stuff, strlen(stuff));
								break;
							case 'm':
								printf("%d asked to convert %s to monochrome\n", pid, filein);
								write(parent_to_mon_pipe[1], stuff, strlen(stuff));
								break;
							default:
								printf("%d sent option '%c' -- there's no such thing\n", pid, option[0]);
								break;
						}

						kill(pid, SIGUSR1);
					}
				}
				//return 0;
				
				//exit(0);
			}
		}
	}

	clean_fifo();
	return 0;
}


void exit_program(int sig) {
	printf("Going to quit\n");
	clean_fifo();
	(void) signal(SIGINT, SIG_DFL);
}

void clean_fifo(){
	close(clients_pipe);
	unlink(NAMED_PIPE);
}