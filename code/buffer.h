// 缓冲区类, 通信文件描述符的读写事件发生时, 将数据读/写到对应的buffer中 暂时只是把这个模块分离出来, 并没有使用
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>

class Buffer
{
private:
    std::string m_buffer;
public:
    Buffer();
    ~Buffer();
    void append(const char* str, int size);
    ssize_t size();
    const char * c_str();
    void clear();
};

#endif // _BUFFER_H_