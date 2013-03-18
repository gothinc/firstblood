#include <fb_core.h>
#include <fb_config.h>

int fb_del_before(char *s, int del, int len){
	if(del > len) return 0;	
	int i;
	for(i = 0; i < (len - del); i ++){
		s[i] = s[i + del];
	}

	s[i] = '\0';
	return 1;
}
