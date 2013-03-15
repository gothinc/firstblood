#include <fb_core.h>
#include <fb_config.h>

int sock_client();
int main(int argc, char *argv[]){
	sock_client();
	return 0;
}

int sock_client(){
	int sock_fd, n;
	char receive_buf[250];
	struct sockaddr_in serv_addr;

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8089);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		return -1;
	}

	while((n = read(sock_fd, receive_buf, sizeof(receive_buf) - 1)) > 0){
		receive_buf[n] = '\0';
		if(fputs(receive_buf, stdout) == EOF){
			printf("print to stdout error\n");
		}
	}

	if(n < 0){
		printf("read error\n");
	}

	return 0;
}
