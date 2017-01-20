#ifndef _NET_H_
#define _NET_H_

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun) || defined(_AIX)
#define AF_LOCAL AF_UNIX
#endif

#ifdef _AIX
#undef ip_len
#endif

int anetTcpConnect(char *err, char *addr, int port);
int anetTcpNonBlockConnect(char *err, char *addr, int port);
int anetRead(int fd, char *buf, int count);
int anetReadTry(int fd, char *buf, int count);
int anetWrite(int fd, char *buf, int count);
int anetWriteTry(int fd, char *buf, int count);
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
int anetTcpServer(char *err, int port,const  char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port,const  char *bindaddr, int backlog);
int anetTcpAccept(char *err, int s, char *ip, size_t ip_len, int *port);

#if !defined WIN32 && !defined __linux__
int anetUnixAccept(char *err, int serversock);
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
int anetUnixConnect(char *err, char *path);
int anetUnixNonBlockConnect(char *err, char *path);
#endif

int anetNonBlock(char *err, int fd);
int anetBlock(char *err, int fd);
int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);
int anetTcpKeepAlive(char *err, int fd);
int anetSendTimeout(char *err, int fd, long long ms);
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
int anetKeepAlive(char *err, int fd, int interval);
int anetSockName(int fd, char *ip, size_t ip_len, int *port);


#endif

