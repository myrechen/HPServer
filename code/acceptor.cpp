#include "acceptor.h"
#include "wrap.h"
#include "channel.h"
#include "tcpserver.h"
#include "tcpconnection.h"

Acceptor::Acceptor(int epfd, int port, const char *IP, int _opt)
{
    m_lfd = getSocketAndBind(port, IP, _opt);   // 获取监听文件描述符

    listenChannel = new Channel(epfd, m_lfd);
    listenChannel->addToEpoll(EPOLLIN);
    // 监听文件描述符发生事件时,是有新连接请求, 回调函数handleNewConnetion()
    listenChannel->readCallback = handleNewConnetion;

    Listen(m_lfd, SOMAXCONN);                   // 服务端开启监听
}

Acceptor::~Acceptor()
{
    printf("关闭监听\n");
    delete listenChannel;
}

int Acceptor::getChannelFd()
{
    return m_lfd;
}

int handleNewConnetion(TcpServer *tcpserver, int epfd, int lfd)
{
    printf("有连接请求\n");
    // 获得新TcpConnection对象
    TcpConnection *clientConnection = new TcpConnection(epfd, lfd);
    // 记录到map中
    int cfd = clientConnection->getChannelFd();
    tcpserver->addTcpConnection(cfd, clientConnection);
}
