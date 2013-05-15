#include <fb_core.h>
#include <fb_config.h>

int
fb_invoke_cgi(char *path, char *buf){
	int pid, n;
	int fd[2];
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

		execl(PHP_PATH, DEFAULT_CGI, path, (char *)0);
	}else{
		close(fd[1]);
		while((n = read(fd[0], tmp, sizeof(tmp) - 1)) > 0){
			tmp[n] = 0;
			strcat(buf,tmp);	
		}

		close(fd[0]);
	}

	return 0;
}
