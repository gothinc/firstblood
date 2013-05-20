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

//ret must be pointer variable(in turn, it is not a char[])
char *link_str(char *pre, char *last, char *ret){
	if(pre == NULL || last == NULL) return (char *) 0;
	ret = (char *)malloc(256);
	strcpy(ret, pre);
	strcat(ret, last);
	return ret;
}

int str_pos(char *s, char f){
	int i;
	for(i = 0; s[i] != 0; i ++){
		if(s[i] == f) return (i + 1);
	}

	return 0;
}

int str_rpos(char *s, char f){
	int i, len;
	len = strlen(s);
	for(i = len - 1; i >= 0; i --){
		if(s[i] == f) return (i + 1);
	}

	return 0;
}
