.SUFFIXES : .o .cpp
OBJS = wprox.o set_mgr.o browser_cb.o accept_cb.o server_cb.o facil.o \
	http_stream.o sock_cb.o
SRCS = wprox.cpp set_mgr.cpp browser_cb.cpp accept_cb.cpp server_cb.cpp \
	facil.cpp http_stream.cpp sock_cb.cpp
CFLAGS = -g
CC = g++

wprox: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	
.cpp.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f *.o
	rm -f wprox
	rm -f *.core

tar:
	cd .. && tar -czf wprox.tgz wprox && mv wprox.tgz wprox/.

	
depend:
	makedepend -- $(SRCS)
