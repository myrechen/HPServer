// epoll相关操作的封装
#ifndef _EPOLLWRAP_H_
#define _EPOLLWRAP_H_

int createEpoll();
void addFdToEpoll(int epfd, int fd, uint32_t events);

#endif // _EPOLLWRAP_H_