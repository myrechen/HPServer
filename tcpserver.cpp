#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include<fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "tcpserver.h"

TcpServer::TcpServer()
{
    m_lfd = socket(PF_INET, SOCK_STREAM, 0);    // 获取监听文件描述符
    
	int opt = 1;
	setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));   //设置端口复用

    struct sockaddr_in serv;
	bzero(&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port = htons(9000);
    bind(m_lfd, (sockaddr*)&serv, sizeof(serv));
    listen(m_lfd, SOMAXCONN);

    m_epfd = epoll_create(1);   // 创建epoll树
    epoll_event ev;
	ev.data.fd = m_lfd;       //将lfd上epoll树
	ev.events = EPOLLIN;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_lfd, &ev);
}

TcpServer::~TcpServer()
{
}

void TcpServer::Run()
{
    int nready;
    int sockfd, cfd;
    epoll_event ev;
    struct epoll_event events[1024];
    char buf[1024];
    
    printf("tcp server 开启\n");
    
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
        {
            sockfd = events[i].data.fd;
            if(sockfd == m_lfd)
            {
                printf("有连接请求\n");
                cfd = accept(m_lfd, NULL, NULL);
                //设置cfd为非阻塞
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);

                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(m_epfd, EPOLL_CTL_ADD, cfd, &ev);
            }
            else if(events[i].events & EPOLLIN) // 可读事件发生
            {
                while (1)
                {
                    memset(buf, 0x00, sizeof(buf));
                    int n = read(sockfd, buf, sizeof(buf));
                    if(n > 0)
                    {
                        printf("n==[%d], buf==[%s]\n", n, buf);
                        for(int k=0; k<n; k++)
                        {
                            buf[k] = toupper(buf[k]);
                        }
                        write(sockfd, buf, n);
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
                        epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, NULL);
                        close(sockfd);
                        break;
                    }
                }
            }
            else
            {
                printf("something else happened\n");
            }
        }
    }
}

