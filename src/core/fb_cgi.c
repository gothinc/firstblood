#include <fb_core.h>
#include <fb_config.h>

int
fb_invoke_cgi(char *path, char *buf, fb_http_req_header_t *req_header_info){
	int pid, n, i;
	int fd[2];
	char *envp[64];
	char *args[3];
	char tmp[1024];
	char port[8];
	char remote_port[8];
	char remote_addr[64];
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
		args[2] = NULL;

		i = 0;
		sprintf(port, "%d", _FB_LISTENED_FD_);
		sprintf(remote_port, "%d", req_header_info->remote_port);
		sprintf(remote_addr, "%d", req_header_info->remote_addr);
		envp[i ++] = "REQUEST_METHOD=GET";
		envp[i ++] = "SERVER_ADDR=127.0.0.1";
		envp[i ++] = link_str("DOCUMENT_ROOT=", _FB_ROOT_PATH);
		envp[i ++] = link_str("SERVER_PORT=", port);
		envp[i ++] = link_str("REMOTE_PORT=", remote_port);
		envp[i ++] = link_str("REMOTE_ADDR=", remote_addr);
		envp[i ++] = link_str("SCRIPT_NAME=", req_header_info->script_name);
		envp[i ++] = link_str("SCRIPT_FILENAME=", link_str(_FB_ROOT_PATH, req_header_info->script_name));
		envp[i ++] = "PATH_TRANSLATED=redirect:/index.php/";
		if(implode_query_string(req_header_info->query_string, query_string)){
			envp[i ++] = link_str("QUERY_STRING=", query_string);
		}else{
			envp[i ++] = "QUERY_STRING=";
		}

		envp[i ++] = link_str("PATH=", getenv("PATH"));
		envp[i ++] = link_str("PATH_INFO=", req_header_info->path_info);
		envp[i ++] = link_str("REQUEST_URI=", req_header_info->request_uri);
		envp[i] = NULL;
		
		execve(PHP_PATH, args, envp);
		exit(1);
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
