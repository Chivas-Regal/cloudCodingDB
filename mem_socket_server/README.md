<h1 align="center">服务端</h1>  

## 文件目录

```
├── CMakeLists.txt
├── README.md
├── backup
│   └── variable_info.txt
├── build
... 生成文件，不再放出来了
├── example
│   └── main.cpp
├── include
│   ├── channel.h
│   ├── config.h
│   ├── containers
│   │   └── trie.h
│   ├── epoll.h
│   ├── eventloop.h
│   ├── inet_address.h
│   ├── limits.h
│   ├── memtools
│   │   ├── memlist.h
│   │   ├── memlist_bf.h
│   │   ├── memlist_ff.h
│   │   ├── memlist_wf.h
│   │   ├── memlistnode.h
│   │   └── mempool.h
│   ├── server.h
│   ├── socket.h
│   ├── threads
│   │   └── threadpool.h
│   ├── usrinfo
│   │   └── sqlmanager.h
│   └── util.h
├── lib
│   ├── libmem_socket_server.dylib
│   ├── libmem_socket_server.so
│   └── libmysqlclient.so
├── src
│   ├── channel.cpp
│   ├── containers
│   │   └── trie.cpp
│   ├── epoll.cpp
│   ├── eventloop.cpp
│   ├── inet_address.cpp
│   ├── memtools
│   │   ├── memlist.cpp
│   │   ├── memlist_bf.cpp
│   │   ├── memlist_ff.cpp
│   │   ├── memlist_wf.cpp
│   │   ├── memlistnode.cpp
│   │   └── mempool.cpp
│   ├── server.cpp
│   ├── socket.cpp
│   ├── threads
│   │   └── threadpool.cpp
│   ├── usrinfo
│   │   └── sqlmanager.cpp
│   └── util.cpp
└── test
    ├── mempool.cpp
    ├── sqlmanager.cpp
    └── thread.cpp
```

## 使用说明

网络与内存的配置信息和与客户端的交互都在 `/include/config.h` 中，其中默认

- `MEM_NUM_LISTS` 内存池空闲链表数量
- `MEM_SIZE_LIST` 内存池一个空闲链表占用空间（单位：字节）
- `MEM_POOL_ALGO` 内存池内存分配所使用的算法
  - `FIRST_FIT` 首次使用算法
  - `BEST_FIT` 最佳适应算法
  - `WORST_FIT` 最坏适应算法
- `SERVER_IP` 服务端监听ip地址
- `SERVER_PORT` 服务端监听端口号
- `SQL_IP` 用户信息数据库连接的ip地址
- `SQL_PORT` 用户信息数据库连接的端口号
- `SQL_USERNAME` 用户信息数据库连接的用户名
- `SQL_PASSWORD` 用户信息数据库连接的密码
- `SQL_DBNAME` 用户信息选择存放数据库名

用户表在使用之前，应当在 `mysql` 对应的库内创建表：

```sql
CREATE TABLE IF NOT EXISTS mem_socket_login_infos (
    username varchar(255) NOT NULL,
    password varchar(255) NOT NULL,
    PRIMARY KEY(username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

程序不存在注册功能，可登入用户信息需数据库管理员手动录入

具体客户端如何与服务端进行联络请参考本仓库另一部分（客户端部分）  
在客户端连接并登录上之后，通过指令向服务端提交变量操作  

## 运行

本工程使用 `cmake` ，终端输入

```txt
cmake -B build
cd build
make
./memserverlib
```

## 功能 与 技术支持

- 以内存利用率为主的内存池设计（连续分区存储管理），支持
    - 空闲节点的切割与合并
    - 空闲链表动态插入节点
- 任意类型数据传输
    - 数据拆分成多个字节发送
    - 收取多个字节后按类型需要的字节数进行合并
- `socket` 进程通信
    - 通过字符串通信模拟报头，完成前置指令
    - 通过 `uint8_t` 传输完成字节信息传递（两个主机之间以大端字节序传输）
- `mysql` 连接下的用户登入信息判断
- 手写容器模板-字典树用于存放 “[用户名][变量名] - 值”
    - 支持 string 与任意类型组成的键值对的存储
    - 支持部分 map<string, 任意类型> 的操作
- 主动下的数据写入主存与读出

## 客户端接入

本服务端客户端通信均以 `send/recv` 方式交互

下面均是客户端应做的事情
- 连接上之后会 `recv` 到一个 OK
- 传入一个字符串代表登入用户的**账号**与**密码**，中间以空格分割，结尾为 '\n'，成功后会 `recv` 到一个 OK
- 传入一个字符串代表操作字符，再传入一个字符串代表操作内容，操作内容都放在一个字符串发送，中间以空格分割，结尾为 '\n'
- **操作字符**
    - 1：表示新建空间，传入**变量名**与**所需要的字节数**，会 `recv` 到一个 OK
    - 2：表示读出数据，传入**变量名**与**偏移首位置**与**偏移尾位置**，会**返回一个 `uint_8` 类型的序列**，会 `recv` 到一个 OK
    - 3：表示写入数据，传入**变量名**与**偏移首位置**与**偏移尾位置**，再**传入一个 `uint_8` 类型的序列**，会 `recv` 到一个 OK
    - 4：表示释放空间，传入**变量名**，会 `recv` 到一个 OK
    - 5：表示程序结束
