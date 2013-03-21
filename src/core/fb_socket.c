#include <fb_core.h>
#include <fb_config.h>

/*regist signal SIGCHLD*/
static void sig_child(int signo);

int fb_open_socket(){
	int listen_fd, i, conn_fd;
	pid_t child_pid;
	fb_sockaddr_in sockaddr;
	
	if((listen_fd = fb_socket(AF_INET, SOCK_STREAM, 0)) == -1) return _FB_ERROR_;
	i = 1;
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &i, sizeof(i) ) < 0) return _FB_ERROR_;

	/*init memory*/
	memset(&sockaddr, 0, sizeof(sockaddr));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(_FB_LISTENED_FD_);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_fd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1) return _FB_ERROR_;
	if(listen(listen_fd, 10) == -1) return _FB_ERROR_;

	/*regist signal SIGCHLD*/
	if(signal(SIGCHLD, sig_child) == SIG_ERR){
		printf("SIGCHLD error\n");
		return -1;
	}
	
	/*daemonize process*/
	fb_daemonize();

	while(1){
		conn_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL);
		if(conn_fd < 0){
			printf("connect error\n");
			break;
		}

		/*fork child process*/
		if((child_pid = fork()) < 0){
			printf("fork error\n");
			break;
		}else if(child_pid == 0){
			/*child process*/

			/*close listen_fd copied from father*/
			if(close(listen_fd) < 0){
				printf("child close listen_fd error\n");
			}

			/*process connection*/
			fb_connect(conn_fd);

			if(close(conn_fd) < 0){
				printf("child close conn_fd error\n");
			}
			exit(0);
		}

		if(close(conn_fd) < 0){
			printf("parent close conn_fd error\n");
		}
	}

	close(listen_fd);	
	return 0;
}

/*process connection*/
void fb_connect(int conn_fd){
	/*http_request_header*/
	fb_http_req_header_t http_req_header;

	/*http_response_header*/
	fb_http_res_header_t http_res_header;

	memset(&http_req_header, '\0', sizeof(http_req_header));
	memset(&http_res_header, '\0', sizeof(http_res_header));

	/*get_request*/
	fb_get_http_request(conn_fd, &http_req_header);

	/*process request, put response*/
	if(http_req_header.path && http_req_header.path[0] == '/'){
		fb_put_http_response(conn_fd, &http_req_header, &http_res_header);
	}else{
		printf("path error\n");
	}

	/*free memory*/
	free_req_header(&http_req_header);
	free_res_header(&http_res_header);
}

/*CATCH SIGCHLD*/
static void sig_child(int signo){
	if(signo != SIGCHLD){
		printf("catch error\n");
	}else{
		if(waitpid(-1, NULL, WNOHANG) <= 0){
			printf("child exit error\n");
		}
	}
}
