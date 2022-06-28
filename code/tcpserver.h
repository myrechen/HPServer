// TCP服务类
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <stdlib.h>
#include <map>

class EventReactor;
class TcpConnection;

class TcpServer
{
private:
    int m_lfd;
    int m_epfd;
    int m_port;

    EventReactor *m_reactor;

    std::map<int, TcpConnection *> m_TcpConnectionMap;  // 保存tcp连接
    
public:
    TcpServer(int port, const char *IP = NULL, int _opt = 1);
    ~TcpServer();
    void Run(); // 启动服务器
    void addTcpConnection(int fd, TcpConnection *newConnection);    // 添加新连接到map
    void closeTcpConnection(int fd);     // 关闭连接并delete
};

int handleNewConnetion(int epfd, int lfd, TcpServer *tcpserver);

#endif // _TCPSERVER_H_
