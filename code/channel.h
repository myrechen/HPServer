#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <sys/epoll.h>

typedef int (* eventCallback)(int epfd, int fd);

class Channel
{
private:
    int m_fd;
    int m_epfd;
    uint32_t m_events;    // 要监控的事件
    uint32_t m_revents;   // 发生的事件
    bool inEpoll;         // 是否在epoll树上

public:
    Channel(int epfd, int fd);
    ~Channel();
    // void setEvents(uint32_t events);    // 设置监控事件
    void addToEpoll(uint32_t events);  // 上epoll树
    int getFd();
    eventCallback readCallback;     // 读回调
    eventCallback writeCallback;    // 写回调
};


#endif // _CHANNEL_H_