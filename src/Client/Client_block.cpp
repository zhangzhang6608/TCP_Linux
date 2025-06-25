
#include "Client_block.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
using namespace std;
#define BUFF_SIZE 1024
static void send_oob(int socket_fd)
{
    char *extern_data = "abc";
    char *nomal_data = "123";
    int ret = -1;
    ret = send(socket_fd, nomal_data, strlen(nomal_data), 0);
    ret = send(socket_fd, extern_data, strlen(extern_data), MSG_OOB);
    if (ret < 0)
    {
        cout << "send oob failed" << endl;
    }
    ret = send(socket_fd, nomal_data, strlen(nomal_data), 0);
};

/**选项二 Sock选项测试  */
static void Sock_Opotion(int socket_fd)
{
    /** 这边是仅设置了Server接受缓冲区的大小，但注意tcp是全双工，也可设置发送缓冲区 */
    size_t RCV_buffsize = 6000, Check_size = 0;
    uint16_t RCV_Paramsize = sizeof(RCV_buffsize);
    setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &RCV_buffsize, sizeof(RCV_buffsize));
    getsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &Check_size, (socklen_t *)RCV_Paramsize);
    cout << "check Size: " << Check_size << endl;
}
/**
 * \brief 持续收写
 * */
static int Continus_send(int connfd, uint8_t times)
{
    char *buff = "testing continous";
    char recv_buff[BUFF_SIZE];
    memset(recv_buff, '\0', BUFF_SIZE);
    int ret = 0;
    while (times != 0)
    {
        ret = send(connfd, buff, strlen(buff), 0);
        if (ret == -1)
        {
            cout << "send error" << endl;
            break;
        }
        usleep(500000);
        ret = recv(connfd, recv_buff, BUFF_SIZE - 1, MSG_DONTWAIT);
        if (ret == -1&&errno!=EAGAIN)
        {
            cout << "receive error\t" <<errno<< endl;
            break;
        }
        cout<<"back message:"<<recv_buff<<endl;
        times--;
    }
    return ret;
};


int Client_Block(int argc, char **argv)
{
    if (argc <= 2)
    {
        cout << "Parameter(IP:Port) is not enough" << endl;
        return -1;
    }
    /**IP参数获取 */
    const char *IP = argv[1];
    int Port = atoi(argv[2]);

    /*地址结构体，指定协议簇及IP,端口等*/
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    inet_pton(AF_INET, IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(Port);

    /*Socket嵌套字描述符*/
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    /*设置发送缓冲区*/
    //Sock_Opotion(sockfd);
    /**connected只表示结果状态,不返回新的嵌套字 */
    int connected = connect(sockfd, (const sockaddr *)&server_addr, sizeof(server_addr));

    if (connected < 0)
    {
        cout << "connected failed" << endl;
    }
    else
    {
        /**带外数据测试 */
        //send_oob(sockfd);
        Continus_send(sockfd,10);
    }

    return 1;
}
