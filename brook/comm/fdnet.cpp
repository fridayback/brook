//#include "StdAfx.h"
#include "fdnet.h"
#include<stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include "platform.h"

static void anetSetError(char *err, const char *fmt, ...)
{
    va_list ap;

    if (!err) return;
    va_start(ap, fmt);
    vsnprintf(err, ANET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int anetSetBlock(char *err, int fd, int non_block) {
   
#ifdef WIN32
	
	if(ioctlsocket(fd,FIONBIO, (u_long FAR*) &non_block) != 0)//∑«◊Ë»˚…Ë÷√
	{
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "ioctlsocket(%d,%d): %s", fd, non_block, tmpstr);
#else
		anetSetError(err, "ioctlsocket(%d,%d): %s",fd, non_block,strerror(errno));
#endif
		return ANET_ERR;
	}
#else
	 int flags;
    /* Set the socket blocking (if non_block is zero) or non-blocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        anetSetError(err, "fcntl(F_GETFL): %s", strerror(errno));
        return ANET_ERR;
    }

    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
        anetSetError(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return ANET_ERR;
    }
#endif
    return ANET_OK;
}

int anetNonBlock(char *err, int fd) {
    return anetSetBlock(err,fd,1);
}

int anetBlock(char *err, int fd) {
    return anetSetBlock(err,fd,0);
}

/* Set TCP keep alive option to detect dead peers. The interval option
 * is only used for Linux as we are using Linux-specific APIs to set
 * the probe send time, interval, and count. */
int anetKeepAlive(char *err, int fd, int interval)
{
    int val = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&val, sizeof(val)) == -1)
    {
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt SO_KEEPALIVE: %s", tmpstr);
#else
		anetSetError(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
#endif
        return ANET_ERR;
    }

#ifdef __linux__
    /* Default settings are more or less garbage, with the keepalive time
     * set to 7200 by default on Linux. Modify settings to make the feature
     * actually useful. */

    /* Send first probe after interval. */
    val = interval;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        anetSetError(err, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return ANET_ERR;
    }

    /* Send next probes after the specified interval. Note that we set the
     * delay as interval / 3, as we send three probes before detecting
     * an error (see the next setsockopt call). */
    val = interval/3;
    if (val == 0) val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        anetSetError(err, "setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));
        return ANET_ERR;
    }

    /* Consider the socket in error state after three we send three ACK
     * probes without getting a reply. */
    val = 3;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        anetSetError(err, "setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
        return ANET_ERR;
    }
#else
    ((void) interval); /* Avoid unused var warning for non Linux systems. */
#endif

    return ANET_OK;
}

static int anetSetTcpNoDelay(char *err, int fd, int val)
{
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val)) == -1)
    {
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt TCP_NODELAY: %s", tmpstr);
#else
		anetSetError(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
#endif
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetEnableTcpNoDelay(char *err, int fd)
{
    return anetSetTcpNoDelay(err, fd, 1);
}

int anetDisableTcpNoDelay(char *err, int fd)
{
    return anetSetTcpNoDelay(err, fd, 0);
}


int anetSetSendBuffer(char *err, int fd, int buffsize)
{
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF,  (const char*)&buffsize, sizeof(buffsize)) == -1)
    {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt SO_SNDBUF: %s", tmpstr);
#else
		anetSetError(err, "setsockopt SO_SNDBUF: %s", strerror(errno));
#endif
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetTcpKeepAlive(char *err, int fd)
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,  (const char*)&yes, sizeof(yes)) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt SO_KEEPALIVE: %s", tmpstr);
#else
		anetSetError(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
#endif
        return ANET_ERR;
    }
    return ANET_OK;
}

/* Set the socket send timeout (SO_SNDTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
int anetSendTimeout(char *err, int fd, long long ms) {
    struct timeval tv;

    tv.tv_sec = ms/1000;
    tv.tv_usec = (ms%1000)*1000;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,  (const char*)&tv, sizeof(tv)) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt SO_SNDTIMEO: %s", tmpstr);
#else
		anetSetError(err, "setsockopt SO_SNDTIMEO: %s", strerror(errno));
#endif
        return ANET_ERR;
    }
    return ANET_OK;
}

/* anetGenericResolve() is called by anetResolve() and anetResolveIP() to
 * do the actual work. It resolves the hostname "host" and set the string
 * representation of the IP address into the buffer pointed by "ipbuf".
 *
 * If flags is set to ANET_IP_ONLY the function only resolves hostnames
 * that are actually already IPv4 or IPv6 addresses. This turns the function
 * into a validating / normalizing function. */
int anetGenericResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len,
                       int flags)
{
    struct addrinfo hints, *info;
    int rv;

    memset(&hints,0,sizeof(hints));
    if (flags & ANET_IP_ONLY) hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(host, NULL, &hints, &info)) != 0) {
        anetSetError(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    if (info->ai_family == AF_INET) {
        struct sockaddr_in *sa = (struct sockaddr_in *)info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, ipbuf_len);
    } else {
        struct sockaddr_in6 *sa = (struct sockaddr_in6 *)info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, ipbuf_len);
    }

    freeaddrinfo(info);
    return ANET_OK;
}

int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return anetGenericResolve(err,host,ipbuf,ipbuf_len,ANET_NONE);
}

int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return anetGenericResolve(err,host,ipbuf,ipbuf_len,ANET_IP_ONLY);
}

static int anetSetReuseAddr(char *err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benckmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,  (const char*)&yes, sizeof(yes)) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt SO_REUSEADDR: %s", tmpstr);
#else
		anetSetError(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
#endif
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anetCreateSocket(char *err, int domain) {
    int s;
    if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "creating socket: %s", tmpstr);
#else
		anetSetError(err, "creating socket: %s", strerror(errno));
#endif
        return ANET_ERR;
    }

    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (anetSetReuseAddr(err,s) == ANET_ERR) {
        CLOSESOCKET(s);
        return ANET_ERR;
    }
    return s;
}

#define ANET_CONNECT_NONE 0
#define ANET_CONNECT_NONBLOCK 1
static int anetTcpGenericConnect(char *err, char *addr, int port, int flags)
{
    int s = ANET_ERR, rv;
    char portstr[6];  /* strlen("65535") + 1; */
    struct addrinfo hints, *servinfo, *p;

#ifdef _WIN32
    _snprintf_s(portstr,sizeof(portstr),"%d",port);
#else
	snprintf(portstr, sizeof(portstr), "%d", port);
#endif
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr,portstr,&hints,&servinfo)) != 0) {
        anetSetError(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;
        if (anetSetReuseAddr(err,s) == ANET_ERR) goto error;
        if (flags & ANET_CONNECT_NONBLOCK && anetNonBlock(err,s) != ANET_OK)
            goto error;
        if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
            /* If the socket is non-blocking, it is ok for connect() to
             * return an EINPROGRESS error here. */
            if (errno == EINPROGRESS && flags & ANET_CONNECT_NONBLOCK)
                goto end;
            CLOSESOCKET(s);
            s = ANET_ERR;
            continue;
        }

        /* If we ended an iteration of the for loop without errors, we
         * have a connected socket. Let's return to the caller. */
        goto end;
    }
    if (p == NULL)
	{
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "creating socket: %s", tmpstr);
#else
		anetSetError(err, "creating socket: %s", strerror(errno));
#endif
	}

error:
    if (s != ANET_ERR) {
        CLOSESOCKET(s);
        s = ANET_ERR;
    }
end:
    freeaddrinfo(servinfo);
    return s;
}

int anetTcpConnect(char *err, char *addr, int port)
{
    return anetTcpGenericConnect(err,addr,port,ANET_CONNECT_NONE);
}

int anetTcpNonBlockConnect(char *err, char *addr, int port)
{
    return anetTcpGenericConnect(err,addr,port,ANET_CONNECT_NONBLOCK);
}

/* Like read(2) but make sure 'count' is read before to return
 * (unless error or EOF condition is encountered) */
int anetRead(int fd, char *buf, int count)
{
    int nread, totlen = 0;
    while(totlen != count) {
#ifdef WIN32
        nread = recv(fd,buf,count-totlen,0);
#else
		 nread = read(fd,buf,count-totlen);
#endif
        if (nread == 0) return totlen;
        if (nread == -1) return -1;
        totlen += nread;
        buf += nread;
    }
    return totlen;
}

int anetReadTry(int fd, char *buf, int count)
{
#ifdef WIN32
	return  recv(fd,buf,count,0);
#else
	return  read(fd,buf,count-totlen);
#endif
}

int anetWriteTry(int fd, char *buf, int count)
{
#ifdef WIN32
	return  send(fd,buf,count,0);
#else
	return  write(fd,buf,count);
#endif
}

/* Like write(2) but make sure 'count' is read before to return
 * (unless error is encountered) */
int anetWrite(int fd, char *buf, int count)
{
    int nwritten, totlen = 0;
    while(totlen != count) {
#ifdef WIN32
		nwritten = send(fd,buf,count-totlen,0);
#else
        nwritten = write(fd,buf,count-totlen);
#endif
        if (nwritten == 0) return totlen;
        if (nwritten == -1) return -1;
        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}

static int anetListen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog) {
    if (bind(s,sa,len) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, GetLastError());
		anetSetError(err, "bind: %s", tmpstr);
#else
		anetSetError(err, "bind: %s", strerror(errno));
#endif
        CLOSESOCKET(s);
        return ANET_ERR;
    }

    if (listen(s, backlog) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "listen: %s", tmpstr);
#else
		anetSetError(err, "listen: %s", strerror(errno));
#endif
        CLOSESOCKET(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anetV6Only(char *err, int s) {
    int yes = 1;
    if (setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,(const char*)&yes,sizeof(yes)) == -1) {
        
#ifdef _WIN32
		char tmpstr[512] = "";
		strerror_s(tmpstr, errno);
		anetSetError(err, "setsockopt: %s", tmpstr);
#else
		anetSetError(err, "setsockopt: %s", strerror(errno));
#endif
        CLOSESOCKET(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int _anetTcpServer(char *err, int port,const char *bindaddr, int af, int backlog)
{
    int s, rv;
    char _port[6];  /* strlen("65535") */
    struct addrinfo hints, *servinfo, *p;

#ifdef _WIN32
    _snprintf_s(_port,6,"%d",port);
#else
	snprintf(_port, 6, "%d", port);
#endif
    memset(&hints,0,sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */

    if ((rv = getaddrinfo(bindaddr,_port,&hints,&servinfo)) != 0) {
        anetSetError(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && anetV6Only(err,s) == ANET_ERR) goto error;
        if (anetSetReuseAddr(err,s) == ANET_ERR) goto error;
        if (anetListen(err,s,p->ai_addr,p->ai_addrlen,backlog) == ANET_ERR) goto error;
        goto end;
    }
    if (p == NULL) {
        anetSetError(err, "unable to bind socket");
        goto error;
    }

error:
    s = ANET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int anetTcpServer(char *err, int port,const  char *bindaddr, int backlog)
{
    return _anetTcpServer(err, port, bindaddr, AF_INET, backlog);
}

int anetTcp6Server(char *err, int port, const char *bindaddr, int backlog)
{
    return _anetTcpServer(err, port, bindaddr, AF_INET6, backlog);
}



static int anetGenericAccept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
    int fd;
    while(1) {
        fd = accept(s,sa,len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
#ifdef _WIN32
				char tmstr[512] = "";
				strerror_s(tmstr, errno);
				anetSetError(err, "accept: %s", tmstr);
#else
				anetSetError(err, "accept: %s", strerror(errno));
#endif
                return ANET_ERR;
            }
        }
        break;
    }
    return fd;
}

int anetTcpAccept(char *err, int s,  char *ip, size_t ip_len, int *port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if ((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return ANET_ERR;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

#if !defined WIN32 && !defined __linux__
int anetUnixAccept(char *err, int s) {
    int fd;
    struct sockaddr_un sa;
    socklen_t salen = sizeof(sa);
    if ((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return ANET_ERR;

    return fd;
}

int anetUnixServer(char *err, char *path, mode_t perm, int backlog)
{
	int s;
	struct sockaddr_un sa;

	if ((s = anetCreateSocket(err,AF_LOCAL)) == ANET_ERR)
		return ANET_ERR;

	memset(&sa,0,sizeof(sa));
	sa.sun_family = AF_LOCAL;
	strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
	if (anetListen(err,s,(struct sockaddr*)&sa,sizeof(sa),backlog) == ANET_ERR)
		return ANET_ERR;
	if (perm)
		chmod(sa.sun_path, perm);
	return s;
}

int anetUnixGenericConnect(char *err, char *path, int flags)
{
	int s;
	struct sockaddr_un sa;

	if ((s = anetCreateSocket(err,AF_LOCAL)) == ANET_ERR)
		return ANET_ERR;

	sa.sun_family = AF_LOCAL;
	strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
	if (flags & ANET_CONNECT_NONBLOCK) {
		if (anetNonBlock(err,s) != ANET_OK)
			return ANET_ERR;
	}
	if (connect(s,(struct sockaddr*)&sa,sizeof(sa)) == -1) {
		if (errno == EINPROGRESS &&
			flags & ANET_CONNECT_NONBLOCK)
			return s;

		anetSetError(err, "connect: %s", strerror(errno));
		close(s);
		return ANET_ERR;
	}
	return s;
}

int anetUnixConnect(char *err, char *path)
{
	return anetUnixGenericConnect(err,path,ANET_CONNECT_NONE);
}

int anetUnixNonBlockConnect(char *err, char *path)
{
	return anetUnixGenericConnect(err,path,ANET_CONNECT_NONBLOCK);
}
#endif


int anetPeerToString(int fd, char *ip, size_t ip_len, int *port) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getpeername(fd,(struct sockaddr*)&sa,&salen) == -1) {
        if (port) *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return 0;
}

int anetSockName(int fd, char *ip, size_t ip_len, int *port) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getsockname(fd,(struct sockaddr*)&sa,&salen) == -1) {
        if (port) *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return 0;
}


