#ifndef _CLIENT_BLOCK_H
#define _CLIENT_BLOCK_H

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * \brief 用于测试发送带外数据
 */
static void send_oob(int socket_fd);

/**
 * \brief 阻塞式Client客户端通信
 */
int Client_Block(int argc, char **argv);

/**
 * \brief 设置通信选项
 */
static void Sock_Opotion(int socket_fd);
#endif