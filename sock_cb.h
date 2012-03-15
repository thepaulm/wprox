#ifndef _SOCK_CB_H
#define _SOCK_CB_H

#include "http_stream.h"
class set_mgr;

class socket_callback
{
public:
    virtual void Fire(set_mgr* pread, set_mgr* pwrite) = 0;

    socket_callback(int s);
    int Close(set_mgr *p);
    int Is_Closed(void);
    int Send(const char *buf, int len);
    ~socket_callback();

protected:
    int m_s;
    http_stream m_stream;
};

#endif /* _SOCK_CB_H */
