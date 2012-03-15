#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "set_mgr.h"
#include "sock_cb.h"

set_mgr::set_mgr()
: max_fd(0)
{
    FD_ZERO(&store_set);
    FD_ZERO(&ready_set);
    memset(m_callbacks, 0, 128 * sizeof(void*));
}

set_mgr::~set_mgr()
{
}

void
set_mgr::add(int fd, socket_callback* pCb)
{
    if (fd > max_fd)
    {
	max_fd = fd;
    }
    m_callbacks[fd] = pCb;
    FD_SET(fd, &store_set);
}

void
set_mgr::remove(int fd)
{
    delete m_callbacks[fd];
    m_callbacks[fd] = 0;
    FD_CLR(fd, &store_set);
}

void
set_mgr::fire(set_mgr* pread, set_mgr* pwrite)
{
    for (int i = 0; i <= max_fd; i++)
    {
	if (FD_ISSET(i, &ready_set))
	{
	    if (m_callbacks[i])
	    {
		m_callbacks[i]->Fire(pread, pwrite);
	    }
	}
    }
}

void
set_mgr::setup()
{
    ready_set = store_set;
}

int
set_mgr::max()
{
    return max_fd;
}

fd_set*
set_mgr::set()
{
    return &ready_set;
}


