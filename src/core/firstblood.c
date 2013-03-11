#include <fb_core.h>
#include <fb_config.h>
#include <firstblood.h>

int main(int argc, char *argv[]){
	#ifdef _LINUX_CONFIG_H_INCLUDED
		printf("%d\n", 1);
	#else
		printf("%d\n", 2);
	#endif
}
