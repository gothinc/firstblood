#ifndef _FB_RESOURCE_CONFIG_H_INCLUDED_

#define _FB_RESOURCE_CONFIG_H_INCLUDED_
void fb_out_put_source(int source_fd, int fd, char *path);
int fb_check_resource(char *path, char *real_path);
int fb_get_404();
int fb_get_file_len(char *path);

#endif
