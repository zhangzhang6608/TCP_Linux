数据提交


git add file1
git commit -m "提交file1的注释"

git add OS
git commit -m "提交OS的注释"

git add .
git commit -m "全部添加解释"

git reset HEAD~1 //撤销本次添加

git push origin main 


# 文件解释
## Server端
 1. Server.main 用于实现具体执行的客户端用法，
 2. Server_Block.cpp 主要是阻塞监听的一些基础用法，其中提供了：带外数据测试，修改缓冲区大小，文件直接传输，管道回传测试等等
 3. Server_Log.cpp 实现日志系统以及一些输出功能
 4. Server_Reactor.cpp 实现Reactor监听模式
 5. Server_Proactor.cpp 实现Proactor监听模式
 6. Server_Signal.cpp 实现统一事件源控制
    * 流程图如下:
    ![unified event source](data/markdown/uni_EventSource.png "统一信号源处理流程")


## Client端
 1. Client_block.cpp 连续重传