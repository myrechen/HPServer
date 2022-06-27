#include <sys/epoll.h>
#include "epollwrap.h"
#include "wrap.h"

int createEpoll()
{
    int epfd = epoll_create(1);
    perr_condition((epfd == -1), "epoll create error");
    return epfd;
}

void addFdToEpoll(int epfd, int fd, uint32_t events)
{//将lfd上epoll树
    epoll_event ev;
	ev.data.fd = fd;       
	ev.events = events;
	perr_condition((epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1), "epoll add error");
}