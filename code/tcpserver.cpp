#include<fcntl.h>
#include "tcpserver.h"
#include "wrap.h"
#include "epollwrap.h"
#include "channel.h"
#include "eventreactor.h"
#include "tcpconnection.h"

TcpServer::TcpServer(int port, const char *IP, int _opt)
{
    m_port = port;
    m_lfd = getSocketAndBind(port, IP, _opt);   // 获取监听文件描述符
    Listen(m_lfd, SOMAXCONN);                   // 服务端开启监听

    m_epfd = createEpoll();                     // 创建epoll树

    m_reactor = new EventReactor(m_epfd, this);       // 创建epoll反应堆, 之后socket文件描述符发生事件由其处理
    
    Channel *listenChannel = new Channel(m_epfd, m_lfd);
    listenChannel->addToEpoll(EPOLLIN);
    // 监听文件描述符发生事件时,是有新连接请求, 回调函数handleNewConnetion()
    listenChannel->readCallback = handleNewConnetion;
}

TcpServer::~TcpServer()
{
}

void TcpServer::Run()
{
    printf("tcp server running at %d\n", m_port);
    // 之后的处理都交给reactor去做
    m_reactor->Run();
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

int handleNewConnetion(int epfd, int lfd, TcpServer *tcpserver)
{
    printf("有连接请求\n");
    // 获得新TcpConnection对象
    TcpConnection *clientConnection = new TcpConnection(epfd, lfd);

    // 记录到map中
    int cfd = clientConnection->getChannelFd();
    tcpserver->addTcpConnection(cfd, clientConnection);
}
