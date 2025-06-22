/**
 * @details Server端
 * @author Zhang zhangzhang
 * @date 2025-6-18 22:00
 */

#include <iostream>
#include <csignal>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include "Server_Block.h"
#include "Server_Log.h"

using namespace std;

#define BUFF_SIZE 1024

static bool is_connected = false;

static void handle_emergency(int client_fd, sockaddr_in client, size_t buff_size)
{

    char buff[buff_size];

    memset(buff, '\0', buff_size);

    int ret = recv(client_fd, buff, buff_size - 1, 0);
    if (ret < 0)
    {
        Output_Waring(client, Faile_Status::RECV_FAILD);
    }
    cout << "Get Bytes:\t" << ret << "\tmessage:\t" << buff << endl;

    /*接受加急外带信息*/
    memset(buff, '\0', buff_size);
    ret = recv(client_fd, buff, buff_size - 1, MSG_OOB);
    if (ret < 0)
    {
        Output_Waring(client, Faile_Status::RECV_FAILD);
    }
    cout << "Get Bytes:\t" << ret << "\tmessage:\t" << buff << endl;

    memset(buff, '\0', buff_size);
    ret = recv(client_fd, buff, buff_size - 1, 0);
    if (ret < 0)
    {
        Output_Waring(client, Faile_Status::RECV_FAILD);
    }
    cout << "Get Bytes:\t" << ret << "\tmessage:\t" << buff << endl;
};
static void handle_connect(int sig)
{
    is_connected = true;
};

int server_block(int argc, char **argv)
{

    signal(SIGTERM, handle_connect); // 实现用户操作，使监听终止
    if (argc <= 2)
    {
        cout << "parameter is necessary: IP and Port" << endl;
        return 0;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = 5; // 最大监听队列长度

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(socket_fd >= 0);

    struct sockaddr_in address;
    bzero(&address, sizeof(sockaddr_in));      // 内存前n个字节清空
    address.sin_family = AF_INET;              /**IPV4协议簇 */
    inet_pton(AF_INET, ip, &address.sin_addr); /**采用IPV4方式，将char*形式的IP地址，转换为二进制的IP形式，同时将结果写入address.sin_addr */
    address.sin_port = htons(port);            // 端口大小端转化-->转为大端形式的网络字节序，

    /*绑定Socket*/
    int ret = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    /*监听*/
    ret = listen(socket_fd, backlog);

    assert(ret != -1);
    /**暂停等待client进行连接*/
    cout << "waiting connected..." << endl;

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int confd = accept(socket_fd, (struct sockaddr *)&client, &client_addrlength);

    if (confd < 0)
    {
        cout << "errno:" << errno << endl;
    }
    else
    {

        Output_Sock(client);
        handle_emergency(confd, client, BUFF_SIZE);
        close(confd);
    }

    close(socket_fd);

    return 0;
};
