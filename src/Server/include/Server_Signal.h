#ifndef _SERVER_SIGNAL_H_
#define _SERVER_SIGNAL_H_

#include "Server_Log.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS_NUMBERS 1024
/**
 * \attention 统一事件源处理,经典的处理方法是，
 * 1. 将信号处理事件放入主循环当中，若有信号触发，则将信号传递给主循环，传递方式一般为管道
 * 2. 主循环根据信号值，执行对应的逻辑代码
 */

/**
 * \brief 设置非阻塞
 */
int setnoblocking(int fd);

/**
 * \brief 向epoll监听队列添加监听事件
 */
void epoll_addfd(int epollfd, int fd);

/**
 * \brief 添加本进程信号量
 */
int addsig(int sig);
/**
 * \brief 信号处理函数
 */

int Server_Signal(int argc, char **argv);

#endif