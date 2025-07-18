#ifndef _SERVER_LOG_H_
#define _SERVER_LOG_H_

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



enum class Faile_Status : uint8_t
{
    CONNECTED_FAILED,
    RECV_FAILD,
};
void handle_connect(int sig);

/**
 * 该部分，预留给之后添加日志系统
 */
void Output_Sock(sockaddr_in sock);

void Output_Waring(sockaddr_in sock, Faile_Status states);

/**
 * \brief 处理参数信息
 * @param Port 参数引用，解析端口
 * @param backlog 参数引用，最大队列长度
 */
char *Prameter(int argc, char **argv, ssize_t &Port, int &backlog);

#endif