
/**
 * @author Zhang Jiahao NWAFU/HFUT
 * @date 2025-05-23 16:00
 * @details 实现Reactor模式的服务端
 */
#include "Server_Reactor.h"
#include "Server_Log.h"

#include <assert.h>
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <string>

/*********************************Reactor函数实例*********************************************** */
Reactor::Reactor()
{

    epollFd_ = epoll_create1(EPOLL_CLOEXEC); // 创建子进程时防泄漏
    if (epollFd_ == -1)
    {
        throw std::runtime_error("epoll_created faild");
    }
    stop_ = false;
};
Reactor::~Reactor()
{
    stop();
    close(epollFd_);
}
void Reactor::start()
{
    this->reactorThread_ = std::thread([this]()
                                       { loop(); });
}

void Reactor::postTask(Task task)
{
    {
        std::lock_guard<std::mutex> lock(taskMutex_);
        taskQueue_.push(std::move(task)); // 移动语义
    }

    cv_.notify_one(); // 唤醒工作线程处理任务
}
void Reactor::registerRead(int fd, Reactor *reactor)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET; // 边缘触发
    ev.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl ADD failed");
        close(fd);
    }
}

void Reactor::modifyEvent(int fd, uint32_t events)
{
    epoll_event ev;
    ev.events = events | EPOLLET;
    ev.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        perror("epoll_ctl MOD failed");
    }
}

void Reactor::handleNewConnection(int fd)
{
    listenFd_.insert(fd);

    sockaddr_in clientAddr;
    socklen_t client_size = sizeof(clientAddr);

    int connfd = accept4(fd, (sockaddr *)&clientAddr, (socklen_t *)&client_size, SOCK_NONBLOCK);

    Output_Sock(clientAddr);

    if (connfd == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return; // 无更多连接
        perror("accept failed");
        return;
    }
    std::cout << "New connection: " << connfd << std::endl;
    // 注册新连接的读事件
    registerRead(connfd, this);
};
Task Reactor::takeTask()
{
    
    std::unique_lock<std::mutex> lock(taskMutex_);  /*获取请求队列的lock，unique_lock离开作用域自动释放*/
    cv_.wait(lock, [this]()
             { return !taskQueue_.empty() || stop_; }); /*等待被触发的条件，若条件未满足则继续等待，避免虚假唤醒*/
    if (stop_ && taskQueue_.empty())
        throw std::runtime_error("Reactor stopped");
    auto task = std::move(taskQueue_.front());      /*移动语义*/
    taskQueue_.pop();
    return task;
}
void Reactor::loop()
{
    std::vector<epoll_event> events(MAX_EVENTS);
    std::cout << "continue wating epoll ....." << std::endl;
    while (!stop_)
    {

        int numEvents = epoll_wait(epollFd_, events.data(), MAX_EVENTS, -1);

        if (numEvents == -1)
        {
            if (errno == EINTR)
                continue; // 被信号中断，继续循环
            perror("epoll_wait failed");
            break;
        }

        // 处理所有触发的事件，生成任务并加入队列
        for (int i = 0; i < numEvents; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t eventMask = events[i].events;
            std::cout << "Process:\t" << eventMask << std::endl;
            // 忽略已关闭的 socket
            if (eventMask & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                close(fd);
                listenFd_.erase(fd);
                continue;
            }

            /* 收到监听Socket的连接，注册的socket事件类除了上述的错误情况只有Pull_in,即监听的新连接到来 */ 

            if (listenFd_.count(fd) != 0)
            {
                handleNewConnection(fd);
            }
            // 已连接Socket，客户端可读（需要读取数据）
            else if (eventMask & EPOLLIN)
            {
                postTask(Task(fd, TaskType::READ, this));
            }
            // 已连接Socket，客户端可写（需要发送数据）
            else if (eventMask & EPOLLOUT)
            {
                postTask(Task(fd, TaskType::WRITE, this));
            }
        }
    }
}
void Reactor::stop()
{
    if (!stop_)
    {
        stop_ = true;
        cv_.notify_all();
        if (reactorThread_.joinable()) /**若线程还是再运行，则阻塞进程，安全退出*/
            reactorThread_.join();
    }
}
void Reactor::set_listen(int sockfd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP; // 监听可读（新连接）、错误、挂起事件
    ev.data.fd = sockfd;                       // 存储套接字描述符（用于后续事件处理）

    int ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, sockfd, &ev);
    if (ret == -1)
    {
        std::cout << "add socket failed" << std::endl;
    }
    listenFd_.insert(sockfd);
}
/*********************************Worker函数实例*********************************************** */
Worker::Worker(Reactor *reactor) : reactor_(reactor), running_(true)
{
    workerThread_ = std::thread([this]()
                                { processTasks(); });
}

Worker::~Worker()
{
    if (workerThread_.joinable())
        workerThread_.join();
}
void Worker::stop()
{
    running_ = false;
    reactor_->stop(); // 工作线程停后,停止 Reactor 线程,防止请求队列累计过多
    if (workerThread_.joinable())
        workerThread_.join();
}
void Worker::processTasks()
{
    while (running_)
    {
        try
        {
            Task task = reactor_->takeTask();
            handleTask(task);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Worker error: " << e.what() << std::endl;
            break;
        }
    }
}
void Worker::handleTask(Task &task)
{
    if (task.type_ == TaskType::READ)
    {
        handleRead(task);
    }
    else if (task.type_ == TaskType::WRITE)
    {
        handleWrite(task);
    }
}
void Worker::handleRead(Task &task)
{
    char buf[4096];
    memset(buf, '\0', 4096);
    ssize_t n = read(task.fd_, buf, 4095);
    if (n > 0)
    {
        std::string back_msg = std::string("Got messgae:").append(buf, n);
        reactor_->postTask(Task(task.fd_, TaskType::WRITE, reactor_, std::vector<char>(back_msg.begin(), back_msg.end()))); // 直接回传消息
    }
    else if (n == 0)
    {
        // 客户端关闭连接
        std::cout << "Client disconnected: " << task.fd_ << std::endl;
        close(task.fd_);
    }
    else
    {
        // 错误处理（EAGAIN 表示无数据可读，正常）
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("read error");
            close(task.fd_);
        }
    }
};

void Worker::handleWrite(Task &task)
{
    ssize_t n = write(task.fd_, task.buff_.data(), task.buff_.size());
    if (n > 0)
    {
        // 发送成功，调整缓冲区（剩余数据）
        if (n < task.buff_.size())
        {
            task.buff_.erase(task.buff_.begin(), task.buff_.begin() + n);
            // 重新注册写事件（继续发送剩余数据）
            reactor_->modifyEvent(task.fd_, EPOLLOUT);
            reactor_->postTask(task); // 重新入队等待下次写机会
            return;
        }
        // 数据全部发送完成，恢复监听读事件
        reactor_->modifyEvent(task.fd_, EPOLLIN);
    }
    else if (n == -1)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("write error");
            close(task.fd_);
        }
    }
}

int Server_Reactor(int argc, char **argv)
{
    int port = 0;
    int backlog = 0; // 最大监听队列长度
    const char *ip = Prameter(argc, argv, port, backlog);
    backlog = 128;
    std::cout << "User Paramer:\t" << ip << "\t Port:\t" << port << std::endl;

    int socket_fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (socket_fd == -1)
    {
        perror("socket failed");
        throw std::runtime_error("Failed to create listen socket");
    }
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt SO_REUSEADDR failed");
        close(socket_fd);
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(sockaddr_in));      // 内存前n个字节清空
    address.sin_family = AF_INET;              /**IPV4协议簇 */
    inet_pton(AF_INET, ip, &address.sin_addr); /**采用IPV4方式，将char*形式的IP地址，转换为二进制的IP形式，同时将结果写入address.sin_addr */
    address.sin_port = htons(port);            // 端口大小端转化-->转为大端形式的网络字节序，

    int ret = bind(socket_fd, (const struct sockaddr *)&address, sizeof(address));

    if (listen(socket_fd, 128) == -1)
    {
        perror("listen failed");
        close(socket_fd);
        throw std::runtime_error("Failed to listen on socket");
    }

    try
    {
        // 启动主线程，监听epoll
        Reactor reactor;
        reactor.set_listen(socket_fd);
        reactor.start();
        // 创建工作线程处理任务
        Worker worker(&reactor);
        while (!User_Quit)
        {
            usleep(10000);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n'
                  << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}