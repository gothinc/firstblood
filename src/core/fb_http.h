#ifndef _FB_HTTP_CONFIG_H_INCLUDED_

#define _FB_HTTP_CONFIG_H_INCLUDED_
typedef struct fb_query_string_s{
	char *key;
	char *value;
} fb_query_string_t;

typedef struct fb_http_req_header_s{
	char *method;
	fb_query_string_t **query_string;
	char *connection;
	char *cookie;
	char *host;
	char *referer;
	char *user_agent;
	char *path;
	char *path_info;
	char *script_name;
	char *request_uri;
	char is_default_path;
	int  remote_port;
	int  remote_addr;
} fb_http_req_header_t;

typedef struct fb_http_res_header_s{
	unsigned short status;
	unsigned short content_length;
	char *connection;
	char *server;
} fb_http_res_header_t;

void fb_get_http_request(int fd, fb_http_req_header_t *header_info);
void fb_parse_http_header(char *buf, fb_http_req_header_t *header_info);
void fb_parse_query_string(char *req, fb_query_string_t **str, int len);
int fb_get_http_header_line(int fd, char *buf, int len);
int read_line(int fd, char *buf, int len);
void fb_put_http_response(int fd, fb_http_req_header_t *req_header_info, fb_http_res_header_t *res_header_info);
void fb_write_res_content(int fd, char *buf, int len);
void free_req_header(fb_http_req_header_t *header_info);
void free_res_header(fb_http_res_header_t *header_info);
void fb_out_put_http_res_status(int fd, int status, char *des);
void fb_out_put_error_header(int fd, int status);
int fb_get_content_type(char *path, char *type);
void fb_send_res_headers(int fd, char *real_path, int len);
int fb_check_img(char *path);
void fb_parse_path_info(fb_http_req_header_t *header_info, char *temp);
void fb_assemble_uri(fb_http_req_header_t * header_info);
int implode_query_string(fb_query_string_t **query_string, char *ret);
char *fb_if_redirect(char *buf);
void fb_send_headers(int fd, char *buf);
void fb_out_put_http_3xx_status(int fd, char *redirect);

#endif
