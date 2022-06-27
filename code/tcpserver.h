// TCP服务类
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <stdlib.h>

class EventReactor;
class TcpServer
{
private:
    int m_lfd;
    int m_epfd;
    int m_port;

    EventReactor *m_reactor;
    
public:
    TcpServer(int port, const char *IP = NULL, int _opt = 1);
    ~TcpServer();
    void Run(); // 启动服务器
};

int handleNewConnetion(int epfd, int lfd);
int handleReadEvent(int epfd, int cfd);

#endif // _TCPSERVER_H_
