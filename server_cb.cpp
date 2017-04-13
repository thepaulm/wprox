#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "browser_cb.h"
#include "facil.h"
#include "server_cb.h"
#include "sock_cb.h"

server_callback::server_callback(int s1, browser_callback *r)
    : socket_callback(s1), p_r(r) {}

void server_callback::Fire(set_mgr *pread, set_mgr *pwrite) {
  int i = 0;
  int j;
  int full;

  full = recv(m_s, m_buffer, READ_BUF, 0);
  if (!full || !p_r || p_r->Is_Closed()) {
    if (debug) {
      printf("closing down socket in read from server ");
      if (!full)
        printf("because server read failed.\n");
      else
        printf("because browser socket closed.\n");
    }
    goto close;
  }

  m_stream.input(m_buffer, full);

  p_r->Send(m_buffer, full);

  if (m_stream.is_stream_end())
    /* XXXPAM possible goto close here?  Don't know.  I
       think they sometimes send multiple requests */
    m_stream.reset();

  return;

close:
  /* this should delete him */
  if (p_r) {
    if (debug)
      printf("server read closing the client read\n");
    p_r->Close(pread);
    p_r = NULL;
  }
  /* this should delete us */
  if (debug)
    printf("server read removing itself\n");

  Close(pread);
}
