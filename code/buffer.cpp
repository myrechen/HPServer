
#include "buffer.h"

Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char* str, int size)
{
    for(int i = 0; i < size; ++i)
    {
        if(str[i] == '\0')
            break;
        m_buffer.push_back(str[i]);
    }
}

ssize_t Buffer::size()
{
    return m_buffer.size();
}

const char * Buffer::c_str()
{
    return m_buffer.c_str();
}

void Buffer::clear()
{
    m_buffer.clear();
}
