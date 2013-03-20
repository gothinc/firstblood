#include <fb_core.h>
#include <fb_config.h>

void fb_out_put_source(int source_fd, int fd){
	int source_num;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	while((source_num = read(source_fd, buf, sizeof(buf) - 1)) > 0){
		buf[source_num] = '\0';
		fb_write_res_content(fd, buf, strlen(buf));
		lseek(source_fd, source_num, SEEK_SET);
	}
}

/*check file exsits and readable*/
int fb_check_resource(char *path, char *real_path){
	if(path[0] == '/'){
		strcat(real_path, _FB_ROOT_PATH);
		strcat(real_path, path);
		return open(real_path, O_RDONLY);
	}else{
		return 0;
	}
}

/*because request file is not exsits, return default 404 page*/
int fb_get_404(){
	return open(_FB_404_PATH, O_RDONLY);
}
