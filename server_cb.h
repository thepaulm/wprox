#include "set_mgr.h"
#include "browser_cb.h"
#include "http_stream.h"

class server_callback : public socket_callback
{
public:
    server_callback(int s1, browser_callback *r);
    void Fire(set_mgr* pread, set_mgr* pwrite);
    
private:
    browser_callback *p_r;
    char m_buffer[READ_BUF];
};
