#include<fcntl.h>
#include <ctype.h>
#include "wrap.h"
#include "tcpconnection.h"
#include "channel.h"
#include "tcpserver.h"

TcpConnection::TcpConnection(int epfd, int lfd)
{
    m_epfd = epfd;
    int cfd;
    cfd = Accept(lfd, NULL, NULL);
    //设置cfd为非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    clientChannel = new Channel(epfd, cfd, this);
    clientChannel->addToEpoll(EPOLLIN);
    // 通信文件描述符发生事件时,有数据发送过来,应该读数据,回调函数handleReadEvent()
    clientChannel->readCallback = handleReadEvent;
    clientChannel->writeCallback = handleWriteEvent;
}

TcpConnection::~TcpConnection()
{
    printf("关闭连接\n");
    delete clientChannel;
}

int TcpConnection::getChannelFd()
{
    return clientChannel->getFd();
}

void TcpConnection::dataProcess()
{
    // 处理数据
    int size = m_readBuffer.size();
    for(int i = 0; i < size; ++i)
    {
        m_readBuffer[i] = toupper(m_readBuffer[i]);
    }

    // 处理完成准备要发送的数据
    m_writeBuffer = m_readBuffer;
    m_readBuffer.clear();
}

int handleReadEvent(TcpServer *tcpserver,  TcpConnection *connection, int epfd, int cfd)
{
    char buf[1024];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        int n = read(cfd, buf, sizeof(buf));
        if(n > 0)
        {
            printf("n==[%d], buf==[%s]\n", n, buf);
            // 将数据放到读缓冲区
            connection->m_readBuffer.append(buf, n);

            // for(int k=0; k<n; k++)
            // {
            //     buf[k] = toupper(buf[k]);
            // }
            // write(cfd, buf, n);
        }
        else if(n == -1 && errno == EINTR)
        {// 系统中断、继续读取
            continue;
        }
        else if(n == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {// 非阻塞IO下这个errno表示读取完毕
            // 处理数据 && 准备要发送的数据
            connection->dataProcess();
            // 让内核监控这个文件描述符的写事件
            connection->clientChannel->updateEvents();
            break;
        }
        else if(n == 0)
        {// 客户端断开连接, 这里需要delete创建TcpConnection Channel对象, 调用对应的析构函数关闭文件描述符
            printf("n==[%d], buf==[%s]\n", n, buf);
            tcpserver->closeTcpConnection(cfd);
            break;
        }
    }
}

int handleWriteEvent(TcpServer *tcpserver,  TcpConnection *connection, int epfd, int cfd)
{
    // 将写缓冲区的数据发送出去
    int size = connection->m_writeBuffer.size();
    Write(connection->getChannelFd(), connection->m_writeBuffer.c_str(), size);

    // 让内核监控这个文件描述符的读事件
    connection->clientChannel->updateEvents();

    connection->m_writeBuffer.clear();
}
