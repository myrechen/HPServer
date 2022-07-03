#include <stdio.h>
// #include <errno.h>
#include "eventreactor.h"
#include "channel.h"
EventReactor::EventReactor(TcpServer *_tcpserver, int epfd)
{
    m_epfd = epfd;
    tcpserver = _tcpserver;
}

EventReactor::~EventReactor()
{
}

void EventReactor::Run()
{
    int nready;
    int sockfd;
    struct epoll_event events[1024];
    Channel *ch;
    
    while(1)
    {
        nready = epoll_wait(m_epfd, events, 1024, -1);
        if(nready < 0)
        {
            perror("epoll_wait error");
            break;
        }

        printf("有事件发生:%d\n", nready);
        for(int i = 0; i < nready; ++i)
        {// 此时reactor不用关心是哪个文件描述符(监听 or 通信)发生事件, 要进行什么操作(新连接 or 读数据), 调用对应的回调函数即可
            ch = (Channel *)events[i].data.ptr;
            sockfd = ch->getFd();
            if(events[i].events == EPOLLIN)
            {
                ch->readCallback(tcpserver, ch->m_connection, m_epfd, sockfd);
            }
            else if(events[i].events == EPOLLOUT)
            {
                ch->writeCallback(tcpserver, ch->m_connection, m_epfd, sockfd);
            }
        }
    }
}

