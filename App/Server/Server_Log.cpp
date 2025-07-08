
#include "Server_Log.h"

bool User_Quit = false;

void handle_connect(int sig)
{
    User_Quit = true;
}

void Output_Sock(sockaddr_in sock)
{
    char IP_buf[sizeof(sock)];
    /*inet_ntop指向缓冲区的，若是转换失败则返回NULL*/
    std::cout << "Sock IP:\t" << inet_ntop(AF_INET, &sock.sin_addr, IP_buf, INET_ADDRSTRLEN) << "\t Port:\t" << ntohs(sock.sin_port) << std::endl;
}

void Output_Waring(sockaddr_in sock, Faile_Status states)
{
    Output_Sock(sock);
    switch (states)
    {
    case Faile_Status::CONNECTED_FAILED:
        /* code */
        std::cout << "failed in connecting process" << std::endl;
        break;
    case Faile_Status::RECV_FAILD:
        std::cout << "failed in receive process" << std::endl;
        break;
    default:
        std::cout << "something failed" << std::endl;
        break;
    }
}

char *Prameter(int argc, char **argv, ssize_t &Port, int &backlog)
{

    if (argc <= 2)
    {
        std::cout << "parameter is necessary: IP and Port" << std::endl;
        return 0;
    }

    char *ip = argv[1];
    Port = atoi(argv[2]);
    backlog = 5; // 最大监听队列长度

    return ip;
}

