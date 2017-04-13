#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "set_mgr.h"
#include "sock_cb.h"

socket_callback::socket_callback(int s) : m_s(s) {}

socket_callback::~socket_callback() { this->Close(NULL); }

int socket_callback::Close(set_mgr *p) {
  int rem;
  if (m_s != -1) {
    close(m_s);
    rem = m_s;
    m_s = -1;
    if (p)
      /* this should delete us */
      p->remove(rem);
  }
  return 0;
}

int socket_callback::Is_Closed(void) {
  if (m_s == -1)
    return 1;
  return 0;
}

int socket_callback::Send(const char *buf, int len) {
  return send(m_s, buf, len, 0);
}
