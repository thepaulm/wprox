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

browser_callback::browser_callback(int s)
    : socket_callback(s), m_bGotURL(0), m_full(0), m_r(-1) {}

void browser_callback::Fire(set_mgr *pread, set_mgr *pwrite) {
  int iGot;
  int i;

  /* take more buffer */
  iGot = recv(m_s, m_buffer + m_full, READ_BUF - m_full, 0);
  if (!iGot)
    goto close;

  if (m_full == READ_BUF && !m_bGotURL)
    goto close;

  m_full += iGot;

  /* make sure we have enough to parse out the url being requested */
  if (!m_bGotURL) {
    for (i = 0; i < m_full; i++) {
      if (m_buffer[i] == '\r' || m_buffer[i] == '\n')
        break;
    }
    if (i == m_full)
      return;
  }

  /* tell our stream tracker about this */
  m_stream.input(m_buffer, m_full);

  /* we actually parse the request here */
  if (!m_bGotURL) {
    int iEOL = i;
    int iStart, iEnd;
    char *p;
    p = strnchr(m_buffer, ' ', iEOL);
    if (!p)
      goto close;
    iStart = p - m_buffer;
    iStart++;
    p = strnchr(m_buffer + iStart, ' ', iEOL - iStart);
    if (!p)
      goto close;
    iEnd = p - m_buffer;
    if (strncasecmp("http://", m_buffer + iStart, 7))
      goto close;
    char hostname[8192];
    i = iStart + 7;
    int j = 0;
    /*
     * host name too long?
     */
    if (iEnd - i >= 8192)
      goto close;
    while (m_buffer[i] != ':' && m_buffer[i] != '/' && m_buffer[i] != '\\' &&
           i < iEnd)
      hostname[j++] = m_buffer[i++];

    hostname[j] = 0;
    unsigned short port = 80;
    if (m_buffer[i] == ':')
      port = atol(m_buffer + i + 1);
    int iURL = i;
    while (iURL < iEnd && m_buffer[iURL] != '/' && m_buffer[iURL] != '\\')
      iURL++;
    if (debug)
      printf("Connecting to %s:%d\n", hostname, port);
    if (m_r != -1) {
      close(m_r);
      pread->remove(m_r);
    }
    m_r = connect_to(hostname, port);
    if (m_r == -1)
      goto close;
    memmove(m_buffer + iStart, m_buffer + iURL, m_full - iURL);
    m_full = m_full - iURL + iStart;
    if (debug) {
      printf("connected to: %s\n", hostname);
      printf("sending request of:\n");
      fwrite(m_buffer, 1, m_full, stdout);
    }
    send(m_r, m_buffer, m_full, 0);
    m_full = 0;
    m_bGotURL = 1;
    pread->add(m_r, new server_callback(m_r, this));
  } else {
    send(m_r, m_buffer, m_full, 0);
    m_full = 0;
  }

  if (m_stream.is_stream_end()) {
    m_bGotURL = 0;
    m_stream.reset();
  }
  return;

close:;
  if (m_r != -1) {
    close(m_r);
    /* this should delete him */
    if (debug)
      printf("client read removing the server\n");
    pread->remove(m_r);
    m_r = -1;
  }
  /* this should delete us */
  if (debug)
    printf("client read closing itself\n");
  Close(pread);
}
