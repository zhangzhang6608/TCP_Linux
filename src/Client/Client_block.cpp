
#include "Client_block.h"
#include <assert.h>
#include <string.h>
using namespace std;

static void send_oob(int socket_fd)
{
    char *extern_data = "abc";
    char *nomal_data = "123";
    int ret = -1;
    ret = send(socket_fd, nomal_data, strlen(nomal_data), 0);
    ret = send(socket_fd, extern_data, strlen(extern_data), MSG_OOB);
    if(ret<0)
    {
        cout<<"send oob failed"<<endl;
    }
    ret = send(socket_fd, nomal_data, strlen(nomal_data), 0);
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

    int connected = connect(sockfd, (const sockaddr *)&server_addr, sizeof(server_addr));

    if (connected < 0)
    {
        cout << "connected failed" << endl;
    }
    else
    {
        /**带外数据测试 */
        send_oob(sockfd);
    }

    return 1;
}
