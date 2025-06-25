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
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include "Server_Block.h"
#include "Server_Log.h"

using namespace std;
extern bool User_Quit;

#define BUFF_SIZE 1024


/**选项一 测试由于TCP分片导致带外数据接受不全 */
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

/**选项二 Sock选项测试  */
static void Sock_Opotion(int socket_fd)
{
    /** 这边是仅设置了Server接受缓冲区的大小，但注意tcp是全双工，也可设置发送缓冲区 */
    size_t RCV_buffsize = 50, Check_size = 0;
    uint16_t RCV_Paramsize = sizeof(RCV_buffsize);
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &RCV_buffsize, sizeof(RCV_buffsize));
    getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &Check_size, (socklen_t *)RCV_Paramsize);
    cout << "check Size: " << Check_size << endl;
}
/**非阻塞式，150ms轮询检测 */
static void Sock_Cycle(int confd, int buffsize)
{
    int ret = -1;
    char *buff[buffsize];
    memset(buff, '\0', buffsize);
    while (!User_Quit)
    {
        ret = recv(confd, buff, buffsize - 1, MSG_DONTWAIT);
        if (ret >= 0)
        {
            cout << "receive data : \t" << buff << endl;
        }
        usleep(150);
    }
};
/**
 *
 * \brief 选项三，sendfile文件传输, 将file_name文件送入到connfd
 *
 */
static int push_file(char *file_name, int connfd)
{
    int file_fd = open(file_name, O_RDONLY);
    assert(file_fd > 0);

    struct stat stat_buff;
    fstat(file_fd, &stat_buff);

    sendfile(connfd, file_fd, NULL, stat_buff.st_size);
}
/**
 * \brief 选项四:回射服务器
 */
static int sendback(int connfd)
{
    int Pipe_fd[2]; /**1写入，0读取 */

    int ret = pipe(Pipe_fd);
    if (ret == -1)
    {
        return ret;
    }
    while (!User_Quit)
    {
        /**先把connfd的数据流入管道 */
        ret = splice(connfd, NULL, Pipe_fd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);

        ret = splice(Pipe_fd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);

        if (ret == -1)
        {
            return ret;
        }
        cout << "waiting Message" << endl;
    }
    close(Pipe_fd[0]);
    close(Pipe_fd[1]);
    return ret;
}


int server_block(int argc, char **argv)
{

    

    int port = 0;
    int backlog = 0; // 最大监听队列长度
    const char *ip = Prameter(argc,argv,port,backlog);
    
    
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(socket_fd >= 0);

    struct sockaddr_in address;
    bzero(&address, sizeof(sockaddr_in));      // 内存前n个字节清空
    address.sin_family = AF_INET;              /**IPV4协议簇 */
    inet_pton(AF_INET, ip, &address.sin_addr); /**采用IPV4方式，将char*形式的IP地址，转换为二进制的IP形式，同时将结果写入address.sin_addr */
    address.sin_port = htons(port);            // 端口大小端转化-->转为大端形式的网络字节序，

    /**设置接受缓冲区大小,在bind之前*/
    //Sock_Opotion(socket_fd);

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
        /*选项一*/
        // handle_emergency(confd, client, BUFF_SIZE);
        /*选项二*/
        // Sock_Cycle(confd, BUFF_SIZE);

        /**选项四：回射服务器*/
        sendback(confd);

        close(confd);
    }

    close(socket_fd);

    return 0;
};
