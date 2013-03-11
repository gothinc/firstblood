CC = gcc
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g
CPP =  gcc -E
LINK = $(CC)

ALL_INCS = -I src/core \
		-I src/os

CORE_DEPS = src/core/fb_config.h \
		src/core/fb_core.h \
		src/core/firstblood.h \
		src/os/fb_linux_config.h

fb: firstblood.o
	$(LINK) -o firstblood firstblood.o \
	-lpthread -lcrypt -lpcre -lcrypto -lcrypto -lz

firstblood.o: $(CORE_DEPS) src/core/firstblood.c
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o firstblood.o src/core/firstblood.c

