// epoll reactor模型
#ifndef _EVENTREACTOR_H_
#define _EVENTREACTOR_H_

class TcpServer;

class EventReactor
{
private:
    int m_epfd;
    TcpServer *tcpserver;
public:
    EventReactor(int epfd, TcpServer *_tcpserver);
    ~EventReactor();
    void Run();     // 进入事件循环
};


#endif // _EVENTREACTOR_H_