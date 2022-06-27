#include "wrap.h"

void perr_condition(bool condition, const char *errmsg)
{// 在条件成立时报错退出
	if(condition){
        perror(errmsg);
        exit(-1);
    }
}
void perr_exit(const char *errmsg)
{
	perror(errmsg);
	exit(-1);
}

int Socket(int family, int type, int protocol)
{
	int n = socket(family, type, protocol);
	if (n < 0)
		perr_exit("socket error");
	return n;
}

int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n = bind(fd, sa, salen);
	if (n < 0)
		perr_exit("bind error");
    return n;
}

int getSocketAndBind(short port, const char *IP, int _opt)
{
	int lfd = Socket(PF_INET, SOCK_STREAM, 0);

	if(_opt == 1)	// 设置端口复用
	{
		int opt = 1;
		setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	}

	struct sockaddr_in serv;
	bzero(&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	if(IP == NULL){// 使用本机任一可用ip
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
    }
	else
	{// 转换失败
		perr_condition((inet_pton(AF_INET,IP,&serv.sin_addr.s_addr) <= 0), IP);
	}
	Bind(lfd, (sockaddr*)&serv, sizeof(serv));

	return lfd;
}

int Listen(int fd, int backlog)
{
    int n = listen(fd, backlog);
	if (n < 0)
		perr_exit("listen error");
    return n;
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

again:
	if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perr_exit("accept error");
	}
	return n;
}

int Close(int fd)
{
    int n = close(fd);
	if (n == -1)
		perr_exit("close error");
    return n;
}

int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n;

	if ((n = connect(fd, sa, salen)) < 0)
		perr_exit("connect error");

    return n;
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ( (n = read(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ( (n = write(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}


static ssize_t my_read(int fd, char *ptr)
{
	static int read_cnt;
	static char *read_ptr;
	static char read_buf[100];//定义了100的缓冲区

	if (read_cnt <= 0) {
again:
        //使用缓冲区可以避免多次从底层缓冲读取数据--为了提高效率
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return -1;
		} else if (read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;//从缓冲区取数据

	return 1;
}
//读取一行
ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char    c, *ptr;

	ptr = (char*)vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c  == '\n')//代表任务完成
				break;
		} else if (rc == 0) {//对端关闭
			*ptr = 0;//0 = '\0'
			return n - 1;
		} else
			return -1;
	}
	*ptr  = 0;

	return n;
}
