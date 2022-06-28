#include<fcntl.h>
#include "tcpserver.h"
#include "wrap.h"
#include "epollwrap.h"
#include "channel.h"
#include "eventreactor.h"
#include "tcpconnection.h"
#include "acceptor.h"

TcpServer::TcpServer(int port, const char *IP, int _opt)
{
    m_port = port;

    m_epfd = createEpoll();                     // 创建epoll树
    m_reactor = new EventReactor(this, m_epfd);       // 创建epoll反应堆, 之后socket文件描述符发生事件由其处理

    acceptor = new Acceptor(m_epfd, port, IP, _opt);    // 创建Acceptor, 监听文件描述符上epoll树, 开启监听, 接受新连接
}

TcpServer::~TcpServer()
{
}

void TcpServer::Run()
{
    printf("tcp server is running at %d\n", m_port);
    m_reactor->Run();   // 之后的处理都交给reactor去做
}

void TcpServer::addTcpConnection(int cfd, TcpConnection *newConnection)
{
    m_TcpConnectionMap[cfd] = newConnection;
}

void TcpServer::closeTcpConnection(int cfd)
{
    TcpConnection *clientConnection = m_TcpConnectionMap[cfd];
    m_TcpConnectionMap.erase(cfd);
    delete clientConnection;    // 调用析构~TcpConnection() --> delete Channel --> 关闭文件描述符
}
