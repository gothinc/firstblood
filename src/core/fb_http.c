#include <fb_core.h>
#include <fb_config.h>

void fb_get_http_request(int fd, fb_http_header_t *header_info){
	int line_len;
	char buf[1024];
	memset(buf, '\0', sizeof(buf));
	do{
		line_len = fb_get_http_header_line(fd, buf, sizeof(buf));
		if(line_len > 0) fb_parse_http_header(buf, header_info);	
	}while(line_len > 0);

	if(header_info->method){
		printf("%s\n", header_info->method);
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

int fb_get_http_header_line(int fd, char *buf, int len){
	char c;
	int n;
	int num = 0;
	while(1){
		n = recv(fd, &c, 1, 0);
		if(n == 0) break;
		if(n < 0){
			if(errno == EINTR || errno == EAGAIN) continue;
			return _FB_ERROR_;
		}

		if(n == 1){
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


