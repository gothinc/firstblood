#include <fb_core.h>
#include <fb_config.h>

void fb_out_put_source(int source_fd, int fd, char *path){
	int source_num, file_len;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	file_len = fb_get_file_len(path);

	while((file_len > 0) && ((source_num = read(source_fd, buf, sizeof(buf) - 1)) > 0)){
		fb_write_res_content(fd, buf, source_num);

		lseek(source_fd, source_num, SEEK_SET);
		file_len -= source_num;
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

int fb_get_file_len(char *path){
	int len = 0;
	FILE *fp;
	fp = fopen(path, "r");
	if(fp != NULL){
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fclose(fp);
	}

	return len;
}

/*because request file is not exsits, return default 404 page*/
int fb_get_404(){
	return open(_FB_404_PATH, O_RDONLY);
}
