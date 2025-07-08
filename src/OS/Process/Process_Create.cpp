
#include "Process_Create.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

void print_pid(void)
{
    cout<<"current pid\t"<<getpid()<<" father pid:\t"<< getppid()<<endl;
}
// 子进程执行的任务函数(示例：计算并打印结果)
void child_task(int task_id) {
    cout<<"current task:\t"<<task_id<<'\t';
    print_pid();

    // 模拟任务执行(计算 1 到 5 的和)
    int sum = 0;
    for (int i = 1; i <= 5; i++) {
        sum += i;
        usleep(100000);  // 模拟耗时操作(100ms)
    }
    cout<<"compute complete"<<task_id<<"\t result:"<<sum<<endl;
    
    exit(0);  // 子进程正常退出(退出码 0),注释这行可观察子进程的开始和终止
}

bool Process_Create(u_int32_t num)
{
    const int num_children = num;  // 子进程数量
    pid_t child_pids[num_children];  // 存储子进程 PID
    cout<<"父进程PID:\t";
    print_pid();
    // 创建多个子进程
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();  // 创建子进程,子进程的起始点:子进程的起始路径

        if (pid == -1) {  // fork 失败
            perror("fork 失败");
            exit(1);
        } else if (pid == 0) {  
            child_task(i );  // 执行子进程任务
            cout<<"子进程继续执行"<<endl;
        } else {  //记录子进程PID
            child_pids[i] = pid;
            cout<<getpid()<<"\t的子进程\t"<<i<<"\t进程号:\t"<<pid<<endl;
        }
    }
    cout<<endl<<endl<<endl;
    cout<<"current:\t"<<getpid()<<endl;
    
    // 父进程等待所有子进程退出
    cout<<"父进程等待子进程退出...\n";
    for (int i = 0; i < num_children; i++) {
        int status;
        pid_t exited_pid = waitpid(child_pids[i], &status, 0);  // 阻塞等待指定子进程

        if (exited_pid == -1) {
            perror("waitpid 失败");
            continue;
        }
        // 解析子进程退出状态
        if (WIFEXITED(status)) {
            printf("子进程 %d(PID=%d)正常退出,退出码：%d\n", 
                   i + 1, exited_pid, WEXITSTATUS(status));
        } else {
            printf("子进程 %d(PID=%d)异常终止\n", i + 1, exited_pid);
        }
    }

    cout<<"当前进程PID:"<<getpid()<<endl;
    return 0;
}
