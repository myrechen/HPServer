// TCP服务器类
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <stdlib.h>

class TcpServer
{
private:
    int m_lfd;
    int m_epfd;
    int m_port;
public:
    TcpServer(int port, const char *IP = NULL, int _opt = 1);
    ~TcpServer();
    void Run();
};


#endif // _TCPSERVER_H_
