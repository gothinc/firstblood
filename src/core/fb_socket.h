#ifndef _FB_SOCKET_CONFIG_H_INCLUDED_

#define _FB_SOCKET_CONFIG_H_INCLUDED_
#define fb_socket socket
#define fb_bind bind
#define _FB_LISTENED_FD_ 8089
typedef struct sockaddr_in fb_sockaddr_in;
void fb_connect(int conn_fd, fb_sockaddr_in client_addr);
int fb_open_socket();

#endif
