/**
 * @author Zhang Jiahao NAWFU/HFUT
 * @date 2025-6-22 10:00
 */

#include "Server_Web.h"
#include <assert.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
using namespace std;
#define BUFF_SIZE 1024

static const char *Status_line[2] = {"200 OK", "500 Internet error"};

static void handle_file(int connfd,char *file_name)
{
    /**处理HTTP带文件传输*/
    char header_buff[BUFF_SIZE];
    memset(header_buff,'\0',BUFF_SIZE);

    /**存放目标文件的内容 */
    char* file_buf;

    struct stat file_stat;

    bool valid = true;

    int len = 0;
    if(stat(file_name,&file_stat)<0)
    {
        valid = false;
    }
    else
    {
        /**st_mode存放一些权限消息 */
        if(S_ISDIR(file_stat.st_mode))
        {
            valid = false;
        }
        else if(file_stat.st_mode& S_IROTH)
        {
            int fd =open(file_name,O_RDONLY); //只读方式
            file_buf = new char[file_stat.st_size+1];
            memset(file_buf, '\0',file_stat.st_size+1);

            if(read(fd,file_buf,file_stat.st_size)<0)
            {
                valid =false;
            }
        }
    }

    if(valid)
    {
        int ret = snprintf(header_buff,BUFF_SIZE-1,"%s %s\r\n", "HTTP/1.1",Status_line[0]);

        struct iovec iv[2];

        iv[0].iov_base = header_buff;
        iv[0].iov_len = strlen(header_buff);
        iv[1].iov_base = file_buf;
        iv[1].iov_len = strlen(file_buf);
    }
    else
    {
        cout<<"error";
    }


}


int Server_Web(int argc, char **argv)
{
    if (argc <= 3)
    {
        cout << "parameter is not enough";
    }

    const char *IP = argv[1];
    int Port = atoi(argv[2]);
    const char *filename = argv[3];

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, IP, &address.sin_addr);
    address.sin_port = htons(Port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);

    assert(ret != -1);
    struct sockaddr_in client;
    socklen_t client_length = sizeof(client);

    int connfd = accept(sock, (struct sockaddr *)&client, &client_length);

    if(connfd<0)
    {
        cout<<"Connected failed"<<endl;
    }
    else
    {
        
    }

    return 0;
}