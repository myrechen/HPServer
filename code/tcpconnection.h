// tcp连接类
#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

class Channel;
class TcpServer;

class TcpConnection
{
private:
    Channel *clientChannel;
    
public:
    TcpConnection(int epfd, int lfd);
    ~TcpConnection();
    int getChannelFd();
};

int handleReadEvent(int epfd, int cfd, TcpServer *tcpserver);

#endif // _TCPCONNECTION_H_