#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *kobe(char **q);

int
main(int argc, char *argv[]){
	enum x{a,b,c,md};
	enum x e;
	e = a;
	printf("%d\n", e);
}

char *kobe(char **q){
	char *s = malloc(16);
	free(s);
	return (char *)0;
}
