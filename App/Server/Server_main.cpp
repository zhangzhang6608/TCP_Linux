/**
 * @author zhang jiahao NAWFU/HFUT
 * @details 控制界面，用于测试所学内容的不同部分
 *
 */

#include "Server_Block.h"
#include "Server_Web.h"
#include "Server_Log.h"
#include "Server_Reactor.h"
#include <csignal>


int main(int argc, char **argv)
{
   signal(SIGTERM, handle_connect); // 实现用户操作，改变监听标志符号，用于终止进程

   //server_block(argc, argv);
   Server_Reactor(argc,argv);

}