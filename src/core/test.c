#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *kobe(char **q);

int
main(int argc, char *argv[]){
	char *s;
	char **q;

	s = kobe(q);
	if(s){
		printf("3\n");
	}else{
		printf("4\n");
	}
}

char *kobe(char **q){
	char *s = malloc(16);
	free(s);
	return (char *)0;
}
