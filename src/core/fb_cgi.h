#ifndef _FB_CORE_CGI_H_INCLUDED_
#define _FB_CORE_CGI_H_INCLUDED_

#define DEFAULT_CGI "PHP"
#define PHP_PATH "/data1/webserver/php/bin/php"

int fb_invoke_cgi(char *path, char *buf, fb_http_req_header_t *req_header_info);

#endif
