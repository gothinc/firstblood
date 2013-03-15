CC = gcc
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g
CPP =  gcc -E
LINK = $(CC)

ALL_INCS = -I src/core \
		-I src/os

CORE_DEPS = src/core/fb_config.h \
		src/core/fb_core.h \
		src/core/firstblood.h \
		src/core/fb_http.h \
		src/core/fb_socket.h \
		src/os/fb_linux_config.h

fb: firstblood.o fb_socket.o fb_http.o
	$(LINK) -o firstblood firstblood.o fb_socket.o fb_http.o \
	-lpthread -lcrypt -lpcre -lcrypto -lcrypto -lz

fb_http.o: src/core/fb_http.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_http.o src/core/fb_http.c

test: test.o 
	$(LINK) -o test test.o \
	-lpthread -lcrypt -lpcre -lcrypto -lcrypto -lz

test.o: src/core/test.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o test.o src/core/test.c

firstblood.o: $(CORE_DEPS) src/core/firstblood.c
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o firstblood.o src/core/firstblood.c

fb_socket.o: $(CORE_DEPS) src/core/fb_socket.c
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_socket.o src/core/fb_socket.c

clean: firstblood.o firstblood
	rm -rf *.o firstblood
