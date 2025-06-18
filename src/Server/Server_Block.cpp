/**
 * @details Server端
 * @author Zhang Jiahao
 * @date 2025-6-18 22:00
 */

#include <iostream>
#include <Server_basic.h>
#include <csignal>
#include <assert.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
using namespace std;

static bool is_connected = false;
static void handle_connect(int sig)
{
    is_connected =true;
}

int main(int argc,char**argv)
{
    
    signal(SIGTERM,handle_connect); //实现用户操作，使监听终止
    const char* ip = "10.2.8.5";
    int port = 3552;
    int backlog = 5;

    int socket_fd = socket(PF_INET,SOCK_STREAM,0);

    assert(socket_fd>=0);

    struct sockaddr_in address;

    bzero(&address,sizeof(sockaddr_in)); //内存前n个字节清空

    address.sin_family = AF_INET;           /**IPV4协议簇 */

    inet_pton(AF_INET,ip,&address.sin_addr);    /**采用IPV4方式，将char*形式的IP地址，转换为二进制的IP形式，同时将结果写入address.sin_addr */

    address.sin_port = htons(port); //端口大小端转化-->转为大端形式的网络字节序，

    int ret = bind(socket_fd,(struct sockaddr*) &address,sizeof(address));

    assert(ret!=0);

    ret =listen(socket_fd,backlog);
    while(!is_connected)
    {
        sleep(1);
    };

    return 0;
}