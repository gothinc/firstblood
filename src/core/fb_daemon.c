#include <fb_core.h>
#include <fb_config.h>

int fb_daemonize(){
	int pid, fd;
	umask(0);

	if((pid = fork()) < 0){
		printf("daemonize fork error\n");
		return -1;
	}else if(pid > 0){
		exit(0);
	}

	if(setsid() == -1){
		printf("setsid error\n");
		return -1;
	}

	if((fd = open("/dev/null", O_RDWR)) == -1){
		printf("open dev null error\n");
		return -1;
	}

	if(dup2(fd, STDIN_FILENO) == -1){
		printf("dup stdin error\n");
		return -1;
	}

	if(dup2(fd, STDOUT_FILENO) == -1){
		printf("dup stdout error\n");
		return -1;
	}

	if(fd > STDERR_FILENO){
		if(close(fd) == -1){
			printf("close dev null error\n");
			return -1;
		}
	}

	return 1;
}
