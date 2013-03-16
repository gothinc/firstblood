#include <fb_core.h>
#include <fb_config.h>

int fb_open_socket(){
	int listen_fd, i;
	fb_sockaddr_in sockaddr;
	
	if((listen_fd = fb_socket(AF_INET, SOCK_STREAM, 0)) == -1) return _FB_ERROR_;
	i = 1;
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &i, sizeof(i) ) < 0) return _FB_ERROR_;

	memset(&sockaddr, '0', sizeof(sockaddr));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(_FB_LISTENED_FD_);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_fd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1) return _FB_ERROR_;
	if(listen(listen_fd, 10) == -1) return _FB_ERROR_;
	while(1){
		fb_connect(listen_fd);
		sleep(1);
	}

	close(listen_fd);	
	return 0;
}

void fb_connect(int listen_fd){
	fb_http_header_t http_header;
	int conn_fd;

	memset(&http_header, '\0', sizeof(http_header));
	conn_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL);
	fcntl(conn_fd, F_SETFL, O_NONBLOCK);

	fb_get_http_request(conn_fd, &http_header);
	if(close(conn_fd) < 0){
		printf("close conn_fd error\n");
		exit(-1);
	}
}
