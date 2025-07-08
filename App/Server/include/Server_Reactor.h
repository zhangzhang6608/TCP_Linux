#ifndef _SERVER_REACTOR_H_
#define _SERVER_REACTOR_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <thread>
#include <queue>
#include <condition_variable>

#include "Server_Log.h"
#include <set>
/*********************************************************************************** */
extern bool User_Quit;
class Reactor;

enum class TaskType
{
    READ,
    WRITE,

};
struct Task
{
    /* data */
    int fd_;
    TaskType type_;
    std::vector<char> buff_;
    Reactor *reactor_;
    Task(int fd, TaskType type, Reactor *reactor, const std::vector<char> &buff = {})
        : fd_(fd), type_(type), reactor_(reactor_), buff_(buff) {};
};

/**
 * \brief Reactor主体部分
 */
class Reactor
{
private:
    static constexpr int MAX_EVENTS = 1024; // 最大同时处理的事件数
    int epollFd_;                           // epoll 实例描述符
    bool stop_ = false;                     // 事件循环停止标志
    std::thread reactorThread_;             // Reactor 线程（事件循环）
    std::queue<Task> taskQueue_;            // 任务队列（主线程 -> 工作线程）
    std::condition_variable cv_;            // 条件变量（通知任务到达）
    std::mutex taskMutex_;                  // 任务队列的互斥锁
    std::set<int> listenFd_;                // 监听 socket 描述符
    void loop();                            // 进行持续队列监听，此处以200ms非阻塞形式监听
    void handleNewConnection(int fd);

public:
    Reactor(); // 创建epoll实例
    ~Reactor();
    void start(); // 启动主线程监听epoll
    void stop();
    void set_listen(int sockfd);
    void postTask(Task task);                    // 将任务发送到请求队列
    void registerRead(int fd, Reactor *reactor); // 向请求队列发送读时间，新连接默认先读
    void modifyEvent(int fd, uint32_t events);   // 更改事件类型

    // 获取任务队列（工作线程调用）
    Task takeTask();
};

/**
 * \brief 工作线程
 */
class Worker
{
private:
    Reactor *reactor_;
    std::thread workerThread_;
    bool running_;
    void processTasks();

public:
    Worker(Reactor *reactor);
    ~Worker();
    void stop();
    void handleTask(Task &task);
    void handleRead(Task &task);
    void handleWrite(Task &task);
};

/**
 * \brief 实现Reactor模式的模拟，主线程负责监听epoll并push请求队列，工作线程负责处理队列中的事件
 */
int Server_Reactor(int argc, char **argv);

#endif