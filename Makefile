CC = gcc
CFLAGS =  -pipe  -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g
CPP =  gcc -E
LINK = $(CC)

ALL_INCS = -I src/core \
		-I src/os

CORE_DEPS = src/core/fb_config.h \
		src/core/fb_core.h \
		src/core/firstblood.h \
		src/core/fb_http.h \
		src/core/fb_socket.h \
		src/core/fb_resource.h \
		src/core/fb_string.h \
		src/core/fb_daemon.h \
		src/core/fb_cgi.h \
		src/os/fb_linux_config.h

fb: firstblood.o fb_socket.o fb_http.o fb_string.o fb_resource.o fb_daemon.o fb_cgi.o
	$(LINK) -g -o firstblood firstblood.o fb_socket.o fb_http.o fb_string.o fb_resource.o fb_daemon.o fb_cgi.o \
	-lpthread -lcrypt -lpcre -lcrypto -lcrypto -lz

fb_cgi.o: src/core/fb_cgi.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_cgi.o src/core/fb_cgi.c

fb_http.o: src/core/fb_http.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_http.o src/core/fb_http.c

fb_daemon.o: src/core/fb_daemon.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_daemon.o src/core/fb_daemon.c

fb_resource.o: src/core/fb_resource.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_resource.o src/core/fb_resource.c

fb_string.o: src/core/fb_string.c $(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_string.o src/core/fb_string.c

firstblood.o: $(CORE_DEPS) src/core/firstblood.c
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o firstblood.o src/core/firstblood.c

fb_socket.o: $(CORE_DEPS) src/core/fb_socket.c
	$(CC) -c $(CFLAGS) $(ALL_INCS) \
	-o fb_socket.o src/core/fb_socket.c

clean: firstblood
	rm -rf *.o firstblood
