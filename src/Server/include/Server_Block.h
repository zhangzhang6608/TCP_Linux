#ifndef _SERVER_BLOCK_H
#define _SERVER_BLOCK_H

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * \brief 用户信号处理函数
 */
static void handle_connect(int sig);

/**
 * \brief 对带外数据进行处理
 */
static void handle_emergency(int client_fd, sockaddr_in client, size_t buff_size);

/**
 * \brief TCP阻塞式IO通信
 */
int server_block(int argc, char **argv);

#endif