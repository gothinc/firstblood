#include <fb_core.h>
#include <fb_config.h>

int fb_check_resource(char *path, char *real_path){
	if(path[0] == '/'){
		strcat(real_path, _FB_ROOT_PATH);
		strcat(real_path, path);
		return open(real_path, O_RDONLY);
	}else{
		return 0;
	}
}

int fb_get_404(){
	return open(_FB_404_PATH, O_RDONLY);
}
