/**
 * @author Zhang Jiahao
 * @date 2025-5-20 14:00
 * @details 不可被引用，只做解释说明使用
 */

#ifndef _CLIENT_BASIC_H
#define _CLIENT_BASIC_H


#include <sys/types.h>
#include <sys/socket.h>


/** 数据发送的一些Flags标志位 */
/**send + recv */
#define EXPAIN_MSG_DONTWAIT MSG_DONTWAIT  //非阻塞式传输
#define EXPAIN_MSG_OOB MSG_OOB              //发送或者接受紧急数据，也称带外数据

/**发送端 send*/
#define EXPAIN_MSG_CONFIRM MSG_CONFIRM //指示数据链路层，一直持续监听到对方的回应，直到收到答复
#define EXPAIN_MSG_DONTROUTE MSG_DONTROUTE //发送方清除目的主机和自身在同一子网，不经过路由表，直接发送到所在子网的目的主机，
#define EXPAIN_MSG_MORE MSG_MORE            //表示还有更多数据要发送
#define EXPAIN_MSG_NOSIGNAL MSG_NOSIGNAL    //往读端关闭的管道或者socket连接中写数据不触发SIGPIPE信号

/**接受端 recv*/
#define EXPAIN_MSG_WAITALL MSG_WAITALL  //仅读到指定字节后才返回
#define EXPAIN_MSG_PEEK MSG_PEEK     //窥探数据，此次操作不会将缓冲区中的指定数据清除



/**
 * \brief 接收Sockfd中的指定大小数据到buf
 * @param buf 数据接受缓存区； 内核数据缓冲区--->buf
 * @param len 期望接受缓冲区大小，但最终数据可能小于这个值
 * @param flags
 * @returns >1:接受成功，返回收到的数据大小; 0:通信双方关闭; -1:接受出错
 */
ssize_t Client_Explain_recv(int sockfd, void *buf, size_t len, int flags)
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
ssize_t Client_Expalin_send(int sockfd,const char *buf,size_t len,int flags)
{
    send(sockfd,buf,len,flags);
}

#endif