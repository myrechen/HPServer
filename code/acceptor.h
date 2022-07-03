// Acceptor 监听获取新连接
#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <stdlib.h>

class Channel;
class TcpServer;
class TcpConnection;

class Acceptor
{
private:
    int m_lfd;
    Channel *listenChannel;
public:
    Acceptor(int epfd, int port, const char *IP = NULL, int _opt = 1);
    ~Acceptor();
    int getChannelFd();
};

int handleNewConnetion(TcpServer *tcpserver,  TcpConnection *connection, int epfd, int lfd);

#endif // _ACCEPTOR_H_