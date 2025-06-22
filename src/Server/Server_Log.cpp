
#include "Server_Log.h"

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