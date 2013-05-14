#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int invoke_cgi(char *path, char *cgi);

int
main(int argc, char *argv[]){
	invoke_cgi("index.php", "php");
}

int
invoke_cgi(char *path, char *cgi){
	int pid, n;
	int fd[2];
	char buf[2048000];
	char tmp[1024];

	if(pipe(fd) != 0){
		printf("pipe error\n");
		return 1;
	}

	if((pid = fork()) < 0){
		printf("fork cgi error\n");
		return 2;
	}else if(pid == 0){
		close(fd[0]);
		if(fd[1] != STDOUT_FILENO){
			if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO){
				printf("dup2 error\n");
				exit(1);
			}

			close(fd[1]);
		}

		execl("/data1/webserver/php/bin/php", cgi, path, (char *)0);
	}else{
		close(fd[1]);
		while((n = read(fd[0], tmp, sizeof(tmp))) != 0){
			strcat(buf,tmp);	
		}

		close(fd[0]);
		printf("%s\n", buf);
		if(waitpid(pid, NULL, 0) < 0){
			printf("waitpid error\n");
			return 1;
		}else{
			printf("child exit\n");
		}

		return 0;
	}
}
