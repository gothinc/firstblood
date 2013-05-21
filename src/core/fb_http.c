#include <fb_core.h>
#include <fb_config.h>

/*process request and put response*/
void fb_put_http_response(int fd, fb_http_req_header_t *req_header_info, fb_http_res_header_t *res_header_info){
	int source_fd;
	char buf[2048000];

	/*request file path*/
	char real_path[128];
	memset(real_path, 0, sizeof(real_path));

	if((source_fd = fb_check_resource(req_header_info->path, real_path)) > 0){
		if(fb_check_img(real_path)){
			/*invoke source_file*/
			fb_out_put_http_res_status(fd, 200);
			fb_send_res_headers(fd, real_path, fb_get_file_len(real_path));
			fb_write_res_content(fd, "\r\n", 2);
			fb_out_put_source(source_fd, fd, real_path);
		}else{
			/*if request file exsits*/
			if(fb_invoke_cgi(real_path, buf, req_header_info) == 0){
				fb_out_put_http_res_status(fd, 200);
				fb_send_res_headers(fd, real_path, strlen(buf));
				fb_write_res_content(fd, "\r\n", 2);
				fb_write_res_content(fd, buf, strlen(buf));
			}else{
				fb_out_put_http_res_status(fd, 404);
				fb_write_res_content(fd, "\r\n", 2);
			}
		}

		close(source_fd);
	}else if((source_fd = fb_get_404()) > 0){
		/*if request file is not exsits*/
		fb_out_put_error_header(fd, 404);

		/*invoke default 404 page*/
		fb_out_put_source(source_fd, fd, _FB_404_PATH);

		close(source_fd);
	}
}

void fb_send_res_headers(int fd, char *real_path, int len){
	char mime_type[256];
	char *ranges = "Accept-Ranges: bytes\r\n";
	char length[64];
	char len_char[8];

	sprintf(len_char, "%d", len);
	strcpy(length, "Content-Length: ");
	strcat(length, len_char);
	strcat(length, "\r\n");

	if(fb_get_content_type(real_path, mime_type)){
		fb_write_res_content(fd, mime_type, strlen(mime_type));
	}

	fb_write_res_content(fd, ranges, strlen(ranges));
	fb_write_res_content(fd, length, strlen(length)); 
}

int fb_check_img(char *path){
	char *type;
	if(path != NULL){
		type = strrchr(path, '.');
		if(strcmp(type, ".jpg") == 0 || strcmp(type, ".jpeg") == 0 || strcmp(type, ".png") == 0 || strcmp(type, ".gif") == 0 || strcmp(type, ".ico") == 0)
			return 1;
	}

	return 0;
}

int fb_get_content_type(char *path, char *header_type){
	char *type;
	if(path != NULL){
		type = strrchr(path, '.');
		if(strcmp(type, ".jpg") == 0 || strcmp(type, ".jpeg") == 0){
			type = "image/jpeg";
		}else if(strcmp(type, ".png") == 0){
			type = "image/png";
		}else if(strcmp(type, ".gif") == 0){
			type = "image/gif";
		}else if(strcmp(type, ".ico") == 0){
			type = "image/ico";
		}else if(strcmp(type, ".php") == 0 || strcmp(type, "html") == 0){
			type = "text/html";
		}else if(strcmp(type, ".swf") == 0){
			type = "application/x-shockwave-flash";
		}else if(strcmp(type, ".js") == 0){
			type = "application/x-javascript";
		}else if(strcmp(type, ".css") == 0){
			type = "text/css";
		}

		strcpy(header_type, "Content-Type: ");
		strcat(header_type, type);
		strcat(header_type, "\r\n");

		return 1;
	}

	return 0;
}

void fb_out_put_error_header(int fd, int status){
	char buf[64];
	memset(buf, 0, sizeof(buf));

	switch(status){
		case 400:
			sprintf(buf, "HTTP/1.1 %d Permission Invalid\r\n", status);
			break;
		case 404:
		default:
			sprintf(buf, "HTTP/1.1 %d Page Not Found\r\n", status);
			break;
	}

	fb_write_res_content(fd, buf, strlen(buf));
	fb_write_res_content(fd, "\r\n", 2);
}

void fb_out_put_http_res_status(int fd, int status){
	char buf[64];
	memset(buf, 0, sizeof(buf));

	if(status == 200){
		sprintf(buf, "HTTP/1.1 %d OK\r\n", status);
	}else if(status == 404){
		sprintf(buf, "HTTP/1.1 %d 404NOT FIND\r\n", status);
	}

	fb_write_res_content(fd, buf, strlen(buf));
}

void fb_write_res_content(int fd, char *buf, int len){
	if(len <= 0 || buf == NULL) return;

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

}

void free_res_header(fb_http_res_header_t *header_info){
	if(header_info->server) free(header_info->server);
	if(header_info->connection) free(header_info->connection);
}

void free_req_header(fb_http_req_header_t *header_info){
	if(header_info->query_string){
		while(*header_info->query_string){
			free((*header_info->query_string)->key);
			free((*header_info->query_string)->value);
			free(*header_info->query_string);
			header_info->query_string ++;
		}
	}

	if(header_info->path) free(header_info->path);
	if(header_info->method) free(header_info->method);
	if(header_info->connection) free(header_info->connection);
	if(header_info->cookie) free(header_info->cookie);
	if(header_info->host) free(header_info->host);
	if(header_info->referer) free(header_info->referer);
	if(header_info->user_agent) free(header_info->user_agent);
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
						if(header_info->query_string == 0) header_info->query_string = (fb_query_string_t **) malloc(sizeof(fb_query_string_t *) * 10);
						fb_parse_query_string(temp, header_info->query_string, strlen(temp));
					}else{
						header_info->query_string = NULL;
					}
				}else if(temp[0] == '/'){
					fb_parse_path_info(header_info, temp);
				}

				j = 0;
			}else if(buf[i] == '?'){
				temp[j] = '\0';
				
				/*todo parse path*/
				if(temp[0] == '/')
					fb_parse_path_info(header_info, temp);

				j = 0;
				temp[j ++] = '~';
			}else{
				temp[j ++] = buf[i];
			}
		}

		fb_assemble_uri(header_info);
		first = 0;
	}else{
		/*todo other header_info*/
	}
}

void fb_assemble_uri(fb_http_req_header_t *header_info){
	char uri[256];
	char query_string[256];
	uri[0] = 0;

	if(!header_info->is_default_path && header_info->path)
		strcpy(uri, header_info->path);

	if(header_info->path_info)
		strcat(uri, header_info->path_info);

	if(header_info->query_string){
		if(implode_query_string(header_info->query_string, query_string))
			strcat(uri, "?");
			strcat(uri, query_string);
	}

	if(uri[0] != 0){
		header_info->request_uri = (char *) malloc(strlen(uri) + 1);
		strcpy(header_info->request_uri, uri);
	}
}

int implode_query_string(fb_query_string_t **query_string, char *ret){
	char s[256];
	s[0] = 0;
	if(!query_string) return 0;
	while(*query_string){
		if((*query_string)->key && (*query_string)->value){
			strcat(s, (*query_string)->key);
			strcat(s, "=");
			strcat(s, (*query_string)->value);
			strcat(s, "&");
		}

		query_string ++;
	}

	if(s[0] != 0) s[strlen(s) - 1] = 0;
	strcpy(ret, s);
	return 1;
}

void fb_parse_path_info(fb_http_req_header_t *header_info, char *temp){
	if(temp[0] != '/') return;

	char path[128];
	char path_info[128];
	int i, j, len;

	if((len = strlen(temp)) == 1){
		header_info->is_default_path = 1;
		header_info->path = (char *) malloc(sizeof(_FB_DEFAULT_PAGE));
		strncpy(header_info->path, _FB_DEFAULT_PAGE, sizeof(_FB_DEFAULT_PAGE));
		header_info->path_info = (char *) malloc(4);
		strcpy(header_info->path_info, "/");
	}else{
		path[0] = '/';
		for(i = 1, len = strlen(temp); i < len && temp[i] != '/'; i ++){
			path[i] = temp[i];
		}

		path[i] = 0;
		
		if(strcmp("/index.php", path) != 0){
			if(strcmp("/login", path) == 0){
				header_info->is_default_path = 1;

				header_info->path = (char *) malloc(sizeof(_FB_DEFAULT_PAGE));
				strncpy(header_info->path, _FB_DEFAULT_PAGE, sizeof(_FB_DEFAULT_PAGE));

				i = 0;
			}else{
				for(; i < len; i ++){
					path[i] = temp[i];
				}

				path[i] = 0;
				header_info->path = (char *) malloc(strlen(path) + 1);
				strncpy(header_info->path, path, strlen(path) + 1);

				return;
			}
		}else{
			header_info->is_default_path = 0;
			header_info->path = (char *) malloc(strlen(path) + 1);
			strncpy(header_info->path, path, strlen(path) + 1);
		}

		if(i < len){
			for(j = 0; i < len; i ++, j ++){
				path_info[j] = temp[i];
			}

			path_info[j] = 0;
			header_info->path_info = (char *) malloc(strlen(path_info) + 1);
			strncpy(header_info->path_info, path_info, strlen(path_info) + 1);
		}
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

