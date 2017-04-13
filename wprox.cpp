#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "accept_cb.h"
#include "facil.h"
#include "set_mgr.h"

#define SO_MAXCONN 128

int debug = 0;

void pipe_handler(int sig) { printf("pipe!\n"); }

void syntax() {
  printf("wprox port [-d][-D]: web proxy (-d for daemon, -D for debug)\n");
}

int main(int argc, char *argv[]) {
  unsigned short port = 0;
  int daemon = 0;
  int s = 0;
  int one = 1;

  for (int x = 0; x < argc; x++) {
    if (argv[x][0] != '-') {
      port = (unsigned short)atoi(argv[x]);
    } else if (argv[x][1] == 'd') {
      daemon = 1;
    } else if (argv[x][1] == 'D') {
      debug = 1;
    }
  }
  if (port == 0) {
    syntax();
    return 0;
  }
  if (daemon)
    daemon_init(argv[0]);
  if (debug)
    printf("Running in debug mode, output to stdout\n");

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  s = socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
    perror("setsockopt");
    return 0;
  }

  if (-1 == bind(s, (sockaddr *)&addr, sizeof(addr))) {
    printf("Failed to bind to %d\n", port);
    return 0;
  }

  if (-1 == listen(s, SO_MAXCONN)) {
    printf("Failed to listen\n");
    return 0;
  }

  signal(SIGPIPE, pipe_handler);
  set_mgr readers;
  readers.add(s, new accept_callback(s));
  int maxn;

  while (1) {
    readers.setup();

    select(readers.max() + 1, readers.set(), 0, 0, 0);
    readers.fire(&readers, 0);
  }

  return 0;
}
