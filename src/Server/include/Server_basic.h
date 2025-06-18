#ifndef _SERVER_BASIC_H
#define _SERVER_BASIC_H


#include<sys/types.h>
#include<sys/socket.h>

#define IPV4 PF_INET
#define IPV6 PF_INET6

#define TCP_TYPE SOCK_STREAM    //字节流服务
#define UDP_TYPE SOCK_DGRAM     //数据报服务

#define NO_BLOCK SOCK_NONBLOCK  //非阻塞
#define FORK_CLOSe SOCK_CLOEXEC //Fork时创建子进程关闭socket

/**
 * \brief 创建一个Socket，只指定服务类型
 * @param domain 表示选用协议，PF_INET为IPV4，PF_INET6为IPV6
 * @param type 表示为上层选用的服务类型, TCP-->SOCK_STREAM, UDP-->SOCK_DGRAM; 新内核情况下，该值可与 非阻塞以及子进程的参数相与&
 * @param protocal 在前面参数的排查下，再去选择一个具体的协议，但基本上可选的只有一个，所以通常都默认为0
 */
int Explain_socket(int domain,int type,int protocol)
{
    /**真正的socket函数*/
    return socket(domain,type,protocol);
}
/**
 * \brief 将Socket指定于嵌套字socketfd，
 * @param socketfd 文件描述符
 * @param addr 指明当前的地址信息
 * @param len socket地址长度
 * @returns 1: 绑定成功； 0: 绑定失败并设置errno-->{EACCES: 表示选用的地址为保护地址(1-1023); EADDRINUS: 绑定的地址为已经被连接占用的地址 }
 */
int Explain_bind(int socketfd, const struct sockaddr * addr, socklen_t len)
{
    return bind(socketfd,addr,len);
}

/**
 * \brief 监听Socket
 * @param socketfd 监听socket指针
 * @param backlog 提示内核最大监听的长度
 */
int Expalin_listen(int socketfd,int backlog)
{
    return listen(socketfd,backlog);
};


#endif