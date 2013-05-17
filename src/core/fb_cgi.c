#include <fb_core.h>
#include <fb_config.h>

int
fb_invoke_cgi(char *path, char *buf, fb_http_req_header_t *req_header_info){
	int pid, n;
	int fd[2];
	char *envp[32];
	char *args[2];
	char tmp[1024];
	char *query_string;

	memset(envp, 0, sizeof(envp));

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
			
		args[0] = DEFAULT_CGI;
		args[1] = path;

		envp[0] = "REQUEST_METHOD=GET";
		envp[1] = "REMOTE_ADDR=127.0.0.1";
		envp[2] = "REMOTE_PORT=8089";
		envp[3] = link_str("DOCUMENT_ROOT=", _FB_ROOT_PATH, envp[3]);
		if((query_string = implode_query_string(req_header_info->query_string)) != NULL){
			envp[4] = link_str("QUERY_STRING=", query_string, envp[4]);
			free(query_string);
		}else{
			envp[4] = "QUERY_STRING=";
		}
		envp[5] = link_str("PATH=", getenv("PATH"), envp[5]);
		envp[6] = NULL;
		
		execve(PHP_PATH, args, envp);
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
