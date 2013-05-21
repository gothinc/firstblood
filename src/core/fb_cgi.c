#include <fb_core.h>
#include <fb_config.h>

int
fb_invoke_cgi(char *path, char *buf, fb_http_req_header_t *req_header_info){
	int pid, n, i;
	int fd[2];
	char *envp[32];
	char *args[2];
	char tmp[1024];
	char query_string[256];

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

		i = 0;
		envp[i ++] = "REQUEST_METHOD=GET";
		envp[i ++] = "REMOTE_ADDR=127.0.0.1";
		envp[i ++] = "REMOTE_PORT=8089";
		if((envp[i] = link_str("DOCUMENT_ROOT=", _FB_ROOT_PATH, envp[i])) != NULL) i ++;
		if(implode_query_string(req_header_info->query_string, query_string)){
			envp[i] = link_str("QUERY_STRING=", query_string, envp[i]);
		}else{
			envp[i] = "QUERY_STRING=";
		}
		i ++;

		if((envp[i] = link_str("PATH=", getenv("PATH"), envp[i])) != NULL) i ++;
		if((envp[i] = link_str("PATH_INFO=", req_header_info->path_info, envp[i])) != NULL) i ++;
		if((envp[i] = link_str("REQUEST_URI=", req_header_info->request_uri, envp[i])) == NULL)
			envp[i] = "REQUEST_URI=";
		i ++;
		envp[i] = NULL;
		
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
