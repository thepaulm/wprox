#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include "facil.h"

/*
 * system functions
 */

/*
 * This is from UNIX Network Programming by
 * W. Richard Stevens, volume 1 second edition, pg 336.
 */
void
daemon_init(const char* pname)
{
    int i;
    pid_t pid;
    if ((pid = fork()) != 0)
    {
	/*
	 * Terminate parent.
	 */
	exit(0);
    }
    
    /*
     * Become session leaders.
     */
    setsid();
    
    signal(SIGHUP, SIG_IGN);
    if ((pid = fork()) != 0)
    {
	/*
	 * 1st child terminates.
	 */
	exit(0);
    }
    
    chdir("/");
    umask(0);
    
    for (i = 0; i < 64; i++)
    {
	close(i);
    }
}

/*
 * network functions.
 */
int
connect_to(char hostname[], unsigned short port)
{
    int s;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(hostname);
    if (addr.sin_addr.s_addr == -1)
    {
	hostent* phe;
	phe = gethostbyname(hostname);
	if (!phe)
	    return -1;
	addr.sin_addr.s_addr = ((in_addr*)(phe->h_addr))->s_addr;
    }
    s = socket(AF_INET, SOCK_STREAM, 0);
    
    if (-1 == connect(s, (sockaddr*)&addr, sizeof(addr)))
    {
	printf("Failed to connect!\n");
	close(s);
	return -1;
    }
    return s;
}

/*
 * mem functions
 */
char*
strnchr(const char* pstr, char c, int n)
{
    while (n--)
    {
	if (!pstr)
	{
	    return NULL;
	}
	if (*pstr == c)
	{
	    return (char*)pstr;
	}
	pstr++;
    }
    return NULL;
}


