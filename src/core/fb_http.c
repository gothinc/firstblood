#include <fb_core.h>
#include <fb_config.h>

void fb_put_http_response(int fd, fb_http_req_header_t *req_header_info, fb_http_res_header_t *res_header_info){
	char buf[1024];				
	memset(buf, 0, sizeof(buf));

	if(strcmp(req_header_info->path, "/index.php") == 0){
		sprintf(buf, "HTTP/1.1 %d OK\r\n", 200);
		fb_write_res_header_line(fd, buf, strlen(buf));
		fb_write_res_header_line(fd, "\r\n", 2);
		sprintf(buf, "<html>\n<head>\n<title>hi,guy</title>\n</head>\n<body>\n<div>\n<p>welcome to zhaojiangwei</p>\n</div>\n</body>\n</html>\n");
		fb_write_res_header_line(fd, buf, strlen(buf));
	}else{
		sprintf(buf, "HTTP/1.1 %d Page Not Found\r\n", 404);
		fb_write_res_header_line(fd, buf, strlen(buf));
		fb_write_res_header_line(fd, "\r\n", 2);
		sprintf(buf, "<html>\n<head>\n<title>404 Page Not Found</title>\n</head>\n<body>\n<div>\n<p>fuck</p>\n</div>\n</body>\n</html>\n");
		fb_write_res_header_line(fd, buf, strlen(buf));
	}
}

void fb_write_res_header_line(int fd, char *buf, int len){
	int ret = 0;
	if((ret = send(fd, buf, len, 0)) <= 0){
		printf("send error\n");
	}
}

void fb_get_http_request(int fd, fb_http_req_header_t *header_info){
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

	if(header_info->query_string){
		while(*header_info->query_string){
			free((*header_info->query_string)->key);
			free((*header_info->query_string)->value);
			free(*header_info->query_string);
			header_info->query_string ++;
		}
	}
}

void fb_parse_http_header(char *buf, fb_http_req_header_t *header_info){
	static int first = 1;
	int len, i, j;
	len = strlen(buf);
	char temp[64];
	memset(temp, '\0', sizeof(temp));

	if(first == 1){
		for(i = 0, j = 0; i < len; i ++){
			if(buf[i] == ' '){
				temp[j] = '\0';
				if(strcmp("GET", temp) == 0){
					header_info->method = (char *) malloc(strlen(temp) + 1);
					strncpy(header_info->method, temp, strlen(temp) + 1);
				}else if(temp[0] == '~'){
					if(strlen(temp) > 1){
						fb_del_before(temp, 1, strlen(temp));
						header_info->query_string = (fb_query_string_t **) malloc(sizeof(fb_query_string_t *) * 10);
						fb_parse_query_string(temp, header_info->query_string, strlen(temp));
					}else{
						header_info->query_string = NULL;
					}
				}

				j = 0;
			}else if(buf[i] == '?'){
				temp[j] = '\0';
				/*todo parse path*/
				header_info->path = (char *) malloc(strlen(temp) + 1);
				strncpy(header_info->path, temp, strlen(temp) + 1);
				j = 0;
				temp[j ++] = '~';
			}else{
				temp[j ++] = buf[i];
			}
		}

		first = 0;
	}else{
		/*todo other header_info*/
	}
}

void fb_parse_query_string(char *req, fb_query_string_t **str, int len){
	int i, j, stage;
	char content[32];
	memset(content, 0, sizeof(content));

	for(i = 0, j = 0, stage = 0; i <= len; i ++){
		if((req[i] == '&') || (i == len)){
			if(stage ==  0) break;//stage == 1:取值阶段,如果==1,则是非法url
			content[j] = '\0';
			strncpy((*str)->value, content, strlen(content) + 1);
			str++;
			j = 0;
			stage = 0;
		}else if(req[i] == '='){
			if(stage == 1) break;//同上
			content[j] = '\0';
			*str = malloc(sizeof(fb_query_string_t));
			(*str)->key = malloc(sizeof(char *));
			(*str)->value = malloc(sizeof(char *));
			strncpy((*str)->key, content, strlen(content) + 1);
			j = 0;
			stage = 1;
		}else{
			content[j ++] = req[i];
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
			//printf("%c", c);
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
