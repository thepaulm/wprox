#ifndef _FACIL_H
#define _FACIL_H
/*
 * system
 */
void daemon_init(const char *pname);
/*
 * network
 */
int connect_to(char hostname[], unsigned short port);
/*
 * mem
 */
char *strnchr(const char *, char, int);

/*
 * for debug messaging
 */
extern int debug;

#endif /* _FACIL_H_ */
