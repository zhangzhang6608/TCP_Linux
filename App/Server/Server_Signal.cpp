
/**
 * \author Zhang Jiahao
 * \date 2025-7-1 15:00
 */
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "Server_Signal.h"

static int sig_pipe[2];

int setnoblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    fcntl(fd, old_option | O_NONBLOCK);
    return old_option;
}
void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(sig_pipe[1], (char *)&msg, 1, 0); /*向管道发送数据*/
    errno = save_errno;
}
/**
 * \brief 信号注册,配置信号处理函数，同时为了防止递归调用，在处理阶段，屏蔽此类信号
 */
int addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;               // 当该信号对应的系统调用处理结束的时候，会重新调用被该系统调用终止的调用
    sigfillset(&sa.sa_mask);                 // 在信号处理阶段时，屏蔽这些信号，防止递归调用
    assert(sigaction(sig, &sa, NULL) != -1); // 为信号进行注册配置
}
void epoll_addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnoblocking(fd);
}

int Server_Signal(int argc, char **argv)
{
    char *IP;
    ssize_t Port = 0;
    int backlog = 0;
    IP = Prameter(argc, argv, Port, backlog);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    // bzero(&addr, sizeof(addr));
    addr.sin_family = PF_INET;
    inet_pton(AF_INET, IP, &addr.sin_addr);
    addr.sin_port = htons(Port);

    int ret = bind(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
    assert(ret != -1);

    /**创建epoll */
    epoll_event events[MAX_EVENTS_NUMBERS];
    int epollfd = epoll_create(5);

    /**创建管道*/
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipe);
    assert(ret != -1);
    setnoblocking(sig_pipe[1]);

    epoll_addfd(epollfd, sig_pipe[0]);

    /*设置信号处理*/
    addsig(SIGHUP);  // 控制终端挂起
    addsig(SIGCHLD); // 子进程状态发生变化，例如退出或者暂停
    addsig(SIGTERM); // 终止进程，kill发送的就是SIGTERM
    addsig(SIGINT);  // 键盘输入以终端进程 ctrl+c

    bool cycle_stop = false;
    while (!cycle_stop)
    {
        int num = epoll_wait(epollfd, events, MAX_EVENTS_NUMBERS, -1);
        if (num < 0 && errno != EINTR)
        {
            std::cout << "epoll failed" << std::endl;
        }
        for (int i = 0; i < num; i++)
        {
            /*判断是否是新连接*/
            int eventfd = events[i].data.fd;
            if (eventfd == socket_fd)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(sockaddr_in);

                int client_fd = accept(socket_fd, (sockaddr *)&client, &len);

                epoll_addfd(epollfd, client_fd);
            }
            else if (eventfd == sig_pipe[0] && (events[i].events & EPOLLIN))
            {
                char buffer[1024];

                ret = recv(sig_pipe[0], buffer, sizeof(buffer), 0);
                if (ret == -1)
                {
                    std::cout << "receive filed";
                }
                else if (ret == 0)
                {
                    continue;
                }
                else
                {
                    // 逐个处理信号
                    for (int i = 0; i < ret; i++)
                    {
                        switch (buffer[0])
                        {
                        case SIGCHLD:
                        case SIGHUP:
                            continue;
                        case SIGTERM:
                        case SIGINT:
                        {
                            cycle_stop = true;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
    std::cout << "close fds\n";
    close(socket_fd);
    close(sig_pipe[1]);
    close(sig_pipe[0]);
    return 0;
}