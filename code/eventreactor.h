// epoll reactor模型
#ifndef _EVENTREACTOR_H_
#define _EVENTREACTOR_H_

class EventReactor
{
private:
    int m_epfd;
public:
    EventReactor(int epfd);
    ~EventReactor();
    void Run();     // 进入事件循环
};


#endif // _EVENTREACTOR_H_