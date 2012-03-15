#ifndef _SET_MGR_H
#define _SET_MGR_H

class socket_callback;

class set_mgr
{
public:
    set_mgr();
    ~set_mgr();
    
    void add(int fd, socket_callback* pCb);
    void remove(int fd);
    void fire(set_mgr* pread, set_mgr* pwrite);
    
    void setup();
    int  max();
    fd_set* set();
    
private:
    fd_set store_set;
    fd_set ready_set;
    int max_fd;
    socket_callback* m_callbacks[128];
};

#endif
