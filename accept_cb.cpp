#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "accept_cb.h"
#include "browser_cb.h"
#include "facil.h"
#include "set_mgr.h"
#include "sock_cb.h"

accept_callback::accept_callback(int s) : socket_callback(s) {}

void accept_callback::Fire(set_mgr *pread, set_mgr *pwrite) {
  int s;
  struct sockaddr_in addr;
  socklen_t len;

  len = sizeof(addr);
  s = accept(m_s, (struct sockaddr *)&addr, &len);
  if (s == -1) {
    if (debug)
      perror("accept_callback->accept");
    return;
  }
  if (debug)
    printf("accepted connection from %s\n", inet_ntoa(addr.sin_addr));
  pread->add(s, new browser_callback(s));
}
