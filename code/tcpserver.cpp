#include<fcntl.h>
#include <ctype.h>
#include "tcpserver.h"
#include "wrap.h"
#include "epollwrap.h"
#include "channel.h"
#include "eventreactor.h"

TcpServer::TcpServer(int port, const char *IP, int _opt)
{
    m_port = port;
    m_lfd = getSocketAndBind(port, IP, _opt);   // 获取监听文件描述符
    Listen(m_lfd, SOMAXCONN);                   // 服务端开启监听

    m_epfd = createEpoll();                     // 创建epoll树

    m_reactor = new EventReactor(m_epfd);       // 创建epoll反应堆, 之后socket文件描述符发生事件由其处理
    
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

int handleNewConnetion(int epfd, int lfd)
{
    int cfd;
    printf("有连接请求\n");
    cfd = Accept(lfd, NULL, NULL);
    //设置cfd为非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    Channel *clientChannel = new Channel(epfd, cfd);
    clientChannel->addToEpoll(EPOLLIN);
    // 通信文件描述符发生事件时,有数据发送过来,应该读数据,回调函数handleReadEvent()
    clientChannel->readCallback = handleReadEvent;
}

int handleReadEvent(int epfd, int cfd)
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
        {//客户端断开连接
            printf("n==[%d], buf==[%s]\n", n, buf);
            //将sockfd对应的事件就节点从epoll树上删除
            epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
            close(cfd);
            break;
        }
    }
}

