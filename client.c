/**
*
*
**/

#include "client.h"
#include "globals.h"

int main(int argc, char *argv[])
{
	(void) signal(SIGINT, exit_program);
	
	if(argc == 4){
		if(argv[1][1] != 'r' && argv[1][1] != 'm' && argv[1][1] != 'c'){
			usage(argv[0]);
			return 0;
		}
		
		//int file = open(argv[2], O_RDONLY);
		//if (file == -1){
		//	perror("Didn't found the input file");
		//	return -1;
		//}else{
		//	close(file);
		//}
	}else{
		usage(argv[0]);
		return 0;
	}
	
	char currentdir[255];
	
	getcwd(currentdir, 255);
	
	
	int daemon_pipe;
	
	while(((daemon_pipe = open(NAMED_PIPE, PIPE_OFLAG, PIPE_SPERM)) == -1) && (errno == EINTR));
	if(daemon_pipe == -1){
		perror("Failed to open named pipe");
		return 1;
	}
	
	char stuff[MAX_PIPE_BUF_SIZE];
	int pid = getpid();
	sprintf(stuff, "%d %c '%s/%s' '%s/%s'", pid, argv[1][1], currentdir, argv[2], currentdir, argv[3]);
	
	write(daemon_pipe, stuff, strlen(stuff));
	
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGUSR2);
	sigaddset(&sigset, SIGINT);
	
	int signo;
	int gotsig;
	
	while((gotsig = sigwait(&sigset, &signo)));
	if(gotsig != -1){
		if(signo == SIGUSR1)
			printf("got SIGUSR1: all good\n");
		else if(signo == SIGUSR2)
			printf("got SIGUSR2: not good\n");
		else if(signo == SIGINT)
			return 0;
	}	

	return 0;
}

void usage(char* prog_name)
{
    fprintf(stderr,"Usage is %s <option> <file-in> <file-out>\n", prog_name);
    fprintf(stderr,"Options\n");
    fprintf(stderr,"  -r   Set resolution to 320x240 px\n");
    fprintf(stderr,"  -m   Convert image to Monochrome\n");
    fprintf(stderr,"  -c   Convert PNG to JPEG\n");
}

void exit_program(int sig) {
	printf("Going to quit\n");
	(void) signal(SIGINT, SIG_DFL);
}
