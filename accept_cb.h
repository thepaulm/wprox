#include "sock_cb.h"

class accept_callback : public socket_callback
{
public:
    accept_callback(int s);
    ~accept_callback();
    void Fire(set_mgr* pread, set_mgr* pwrite);
    
};
