#ifndef BROWSER_CB_H
#define BROWSER_CB_H

#include "http_stream.h"
#include "sock_cb.h"

#define READ_BUF 65536

class browser_callback : public socket_callback {
public:
  browser_callback(int s);
  void Fire(set_mgr *pread, set_mgr *pwrite);

private:
  int m_r;
  char m_buffer[READ_BUF];
  int m_full;
  int m_bGotURL;
};

#endif /* BROWSER_CB_H */
