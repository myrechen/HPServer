#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

class TcpServer
{
private:
    int m_lfd;
    int m_epfd;
    int m_port;
public:
    TcpServer();
    ~TcpServer();
    void Run();
};


#endif // _TCPSERVER_H_
