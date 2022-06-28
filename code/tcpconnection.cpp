#include<fcntl.h>
#include <ctype.h>
#include "wrap.h"
#include "tcpconnection.h"
#include "channel.h"
#include "tcpserver.h"

TcpConnection::TcpConnection(int epfd, int lfd)
{
    int cfd;
    cfd = Accept(lfd, NULL, NULL);
    //设置cfd为非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    clientChannel = new Channel(epfd, cfd);
    clientChannel->addToEpoll(EPOLLIN);
    // 通信文件描述符发生事件时,有数据发送过来,应该读数据,回调函数handleReadEvent()
    clientChannel->readCallback = handleReadEvent;
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

int handleReadEvent(TcpServer *tcpserver, int epfd, int cfd)
{
    char buf[1024];
    while (1)
    {
        memset(buf, 0x00, sizeof(buf));
        int n = read(cfd, buf, sizeof(buf));
        if(n > 0)
        {
            printf("n==[%d], buf==[%s]\n", n, buf);
            for(int k=0; k<n; k++)
            {
                buf[k] = toupper(buf[k]);
            }
            write(cfd, buf, n);
        }
        else if(n == -1 && errno == EINTR)
        {// 系统中断、继续读取
            continue;
        }
        else if(n == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {// 非阻塞IO下这个errno表示读取完毕
            break;
        }
        else if(n == 0)
        {//客户端断开连接, 这里需要delete创建TcpConnection Channel对象, 调用对应的析构函数关闭文件描述符
            printf("n==[%d], buf==[%s]\n", n, buf);
            tcpserver->closeTcpConnection(cfd);
            break;
        }
    }
}
