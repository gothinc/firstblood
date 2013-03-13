#include <fb_core.h>
#include <fb_config.h>
#include <fb_socket.h>

int fb_open_socket(){
	int listen_fd, conn_fd, wt;
	char send_buf[255];
	struct sockaddr_in sockaddr;
	time_t ticks;
	
	if((listen_fd = fb_socket(AF_INET, SOCK_STREAM, 0)) == -1) return _FB_ERROR_;
	memset(send_buf, '0', sizeof(send_buf));		
	memset(&sockaddr, '0', sizeof(sockaddr));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(_FB_LISTENED_FD_);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_fd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1) return _FB_ERROR_;
	if(listen(listen_fd, 10) == -1) return _FB_ERROR_;
	while(1){
		conn_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL);
		ticks = time(NULL);
		snprintf(send_buf, sizeof(send_buf), "%.24s\r\n", ctime(&ticks));
		if((wt = write(conn_fd, send_buf, strlen(send_buf))) < 0) return -1;

		close(conn_fd);
		sleep(1);
	}

	close(listen_fd);	
}
