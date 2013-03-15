#ifndef _FB_HTTP_CONFIG_H_INCLUDED_

#define _FB_HTTP_CONFIG_H_INCLUDED_
typedef struct fb_http_header_s{
	int status;
	char *method;
	char *query_string;
} fb_http_header_t;

void fb_get_http_request(int fd, fb_http_header_t *header_info);
void fb_parse_http_header(char *buf, fb_http_header_t *header_info);
int fb_get_http_header_line(int fd, char *buf, int len);

#endif
