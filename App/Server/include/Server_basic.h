/**
 * @details 该头文件不可被引用,只做学习解释说明用
 */

#ifndef _SERVER_BASIC_H
#define _SERVER_BASIC_H

#include <sys/types.h>
#include <sys/socket.h>

#define IPV4 PF_INET
#define IPV6 PF_INET6

#define TCP_TYPE SOCK_STREAM // 字节流服务
#define UDP_TYPE SOCK_DGRAM  // 数据报服务

#define NO_BLOCK SOCK_NONBLOCK  // 非阻塞
#define FORK_CLOSe SOCK_CLOEXEC // Fork时创建子进程关闭socket

/** 数据发送的一些Flags标志位 */
/**send + recv */
#define EXPAIN_MSG_DONTWAIT MSG_DONTWAIT // 非阻塞式传输
#define EXPAIN_MSG_OOB MSG_OOB           // 发送或者接受紧急数据，也称带外数据

/**发送端 send*/
#define EXPAIN_MSG_CONFIRM MSG_CONFIRM     // 指示数据链路层，一直持续监听到对方的回应，直到收到答复
#define EXPAIN_MSG_DONTROUTE MSG_DONTROUTE // 发送方清除目的主机和自身在同一子网，不经过路由表，直接发送到所在子网的目的主机，
#define EXPAIN_MSG_MORE MSG_MORE           // 表示还有更多数据要发送
#define EXPAIN_MSG_NOSIGNAL MSG_NOSIGNAL   // 往读端关闭的管道或者socket连接中写数据不触发SIGPIPE信号

/**接受端 recv*/
#define EXPAIN_MSG_WAITALL MSG_WAITALL // 仅读到指定字节后才返回
#define EXPAIN_MSG_PEEK MSG_PEEK       // 窥探数据，此次操作不会将缓冲区中的指定数据清除

/**
 * \brief 创建一个Socket，只指定服务类型
 * @param domain 表示选用协议，PF_INET为IPV4，PF_INET6为IPV6
 * @param type 表示为上层选用的服务类型, TCP-->SOCK_STREAM, UDP-->SOCK_DGRAM; 新内核情况下，该值可与 非阻塞以及子进程的参数相与&
 * @param protocal 在前面参数的排查下，再去选择一个具体的协议，但基本上可选的只有一个，所以通常都默认为0
 */
int Explain_socket(int domain, int type, int protocol)
{
    /**真正的socket函数*/
    return socket(domain, type, protocol);
}
/**
 * \brief 将Socket指定于嵌套字socketfd，
 * @param socketfd 文件描述符
 * @param addr 指明当前的地址信息
 * @param len socket地址长度
 * @returns 0: 绑定成功； -1: 绑定失败并设置errno-->{EACCES: 表示选用的地址为保护地址(1-1023); EADDRINUS: 绑定的地址为已经被连接占用的地址 }
 */
int Explain_bind(int socketfd, const struct sockaddr *addr, socklen_t len)
{
    return bind(socketfd, addr, len);
}

/**
 * \brief 监听Socket
 * @param socketfd 监听socket指针
 * @param backlog 提示内核最大监听的长度,通过这个参数可设置Server连接的最大个数
 * @return 0: 绑定成功； -1: 绑定失败并设置errno-->{EACCES: 表示选用的地址为保护地址(1-1023); EADDRINUS: 绑定的地址为已经被连接占用的地址 }
 */
int Expalin_listen(int socketfd, int backlog)
{
    return listen(socketfd, backlog);
};

/**
 * \brief 接收Sockfd中的指定大小数据到buf
 * @param buf 数据接受缓存区； 内核数据缓冲区--->buf
 * @param len 期望接受缓冲区大小，但最终数据可能小于这个值
 * @param flags
 * @returns >1:接受成功，返回收到的数据大小; 0:通信双方关闭; -1:接受出错
 */
ssize_t Server_Explain_recv(int sockfd, void *buf, size_t len, int flags)
{
    recv(sockfd, buf, len, flags);
}

/**
 * \brief 向对方发送数据
 * @param buf 数据发送缓存区； buf-->内核数据发送缓冲区
 * @param len 发送数据大小
 * @param flags
 * @returns 成功则返回实际传输的数据，失败则返回-1，并设置errno
 */
ssize_t Server_Expalin_send(int sockfd, const char *buf, size_t len, int flags)
{
    send(sockfd, buf, len, flags);
}

#endif