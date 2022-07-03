// tcp连接类
#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include <string>

class Channel;
class TcpServer;

class TcpConnection
{
private:
    int m_epfd;

    Channel *clientChannel;
    std::string m_readBuffer;
    std::string m_writeBuffer;
    
public:
    TcpConnection(int epfd, int lfd);
    ~TcpConnection();
    int getChannelFd();

    void dataProcess(); // 处理读缓冲区的数据
    friend int handleReadEvent(TcpServer *tcpserver, TcpConnection *connection, int epfd, int cfd);
    friend int handleWriteEvent(TcpServer *tcpserver, TcpConnection *connection, int epfd, int cfd);
};

int handleReadEvent(TcpServer *tcpserver, TcpConnection *connection, int epfd, int cfd);
int handleWriteEvent(TcpServer *tcpserver, TcpConnection *connection, int epfd, int cfd);

#endif // _TCPCONNECTION_H_