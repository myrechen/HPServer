#include "tcpserver.h"

int main()
{
    TcpServer tcpserver(8888);
    tcpserver.Run();
    
    return 0;
}

