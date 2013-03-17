#include <fb_core.h>
#include <fb_config.h>

void fb_get_http_request(int fd, fb_http_header_t *header_info){
	int line_len, recv;
	char buf[1024];

	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	memset(buf, '\0', sizeof(buf));

	while(1){
		if(FD_ISSET(fd, &rset) == 0) FD_SET(fd, &rset);
		recv = select(fd + 1, &rset, NULL, NULL, &tv);
		if(recv > 0){
			if((line_len = read_line(fd, buf, sizeof(buf))) > 0){
				fb_parse_http_header(buf, header_info);
			}else{
				break;
			}
		}else if(recv == 0){
			printf("select recive null\n");
			break;
		}else{
			printf("select error\n");
			break;
		}
	}
}

void fb_parse_http_header(char *buf, fb_http_header_t *header_info){
	int len, i, j;
	len = strlen(buf);
	char temp[64];
	memset(temp, '\0', sizeof(temp));
	for(i = 0, j = 0; i < len; i ++){
		if(buf[i] == ' '){
			temp[j] = '\0';
			if(strcmp("GET", temp) == 0){
				header_info->method = (char *) malloc(strlen(temp) + 1);
				strncpy(header_info->method, temp, strlen(temp) + 1);
			}

			j = 0;
		}else{
			temp[j ++] = buf[i];
		}
	}
}

int read_line(int fd, char *buf, int len){
	char c;
	int n;
	int num = 0;
	while(1){
		n = recv(fd, &c, 1, 0);
		if(n == 0) break;
		if(n < 0){
			if(errno == EINTR) continue;
			return _FB_ERROR_;
		}

		if(n == 1){
			printf("%c", c);
			if(c == '\r') continue;
			if(c == '\n') break;
			buf[num ++] = c;
		}else{
			return _FB_ERROR_;
		}
	}
	
	buf[num] = '\0';
	return num;
}
