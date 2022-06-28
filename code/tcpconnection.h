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

int handleReadEvent(TcpServer *tcpserver, int epfd, int cfd);

#endif // _TCPCONNECTION_H_