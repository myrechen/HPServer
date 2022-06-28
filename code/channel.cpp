#include "channel.h"
#include "wrap.h"

Channel::Channel(int epfd, int fd)
{
    m_epfd = epfd;
    m_fd = fd;
    m_events = 0;
    m_revents = 0;
    inEpoll = false;
}

Channel::~Channel()
{
    //将sockfd对应的事件就节点从epoll树上删除
    printf("关闭channel:[%d]\n", m_fd);
    epoll_ctl(m_epfd, EPOLL_CTL_DEL, m_fd, NULL);
    close(m_fd);
}

// void Channel::setEvents(uint32_t events)
// {
//     m_events = events;
// }

void Channel::addToEpoll(uint32_t events)
{
    m_events = events;
    struct epoll_event ev;
    ev.data.ptr = this;
    ev.events = events;
    if(!inEpoll)
    {
        int ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_fd, &ev);
        perr_condition(ret == -1, "epoll add error");
        inEpoll = true;
    }
}

int Channel::getFd()
{
    return m_fd;
}

