#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <sys/epoll.h>

class TcpServer;
class TcpConnection;

typedef int (* eventCallback)(TcpServer *tcpserver,  TcpConnection *connection, int epfd, int cfd);

class Channel
{
private:
    int m_fd;
    int m_epfd;
    uint32_t m_events;    // 要监控的事件
    uint32_t m_revents;   // 发生的事件
    bool inEpoll;         // 是否在epoll树上

public:
    TcpConnection *m_connection;
    Channel(int epfd, int fd, TcpConnection *connection);
    ~Channel();
    // void setEvents(uint32_t events);    // 设置监控事件
    void addToEpoll(uint32_t events);  // 上epoll树
    void updateEvents();    // 修改要监控的事件

    int getFd();
    eventCallback readCallback;     // 读回调
    eventCallback writeCallback;    // 写回调
};


#endif // _CHANNEL_H_