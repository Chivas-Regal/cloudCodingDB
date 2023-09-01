<h1 align="center">云端存储型编程向KV数据库</h1>

**本项目正在疯狂迭代中**  
**I'm rushing B !!!**  

一个以 lru(in mempool) + memtable + lsmtree 存储、以 socket(in threadpool) 网络通信的 KV 数据库

![20230731225733](https://cr-demo-blog-1308117710.cos.ap-nanjing.myqcloud.com/chivas-regal/20230731225733.png)

## code-api 使用演示

支持利用客户端用写好的通信工具 Connector 完成操作

[前往YouTube观看](https://www.youtube.com/watch?v=19KL01TNFf8)  

## local-cmd 使用演示

这里是本地运行时直接使用命令完成 KV 操作  

[前往YouTube观看](https://www.youtube.com/watch?v=dqNmS3SIFl0)  

这里我们写好的数值都可以在后面利用到来完成编程向数据库  
**注意这里数据的申请是不费我们自己的内存的**

## 功能 与 技术支持

- 存储
  - LSM-tree + SST 磁盘存储
      - SST 文件的编码与解码以及刷盘
      - LSM-tree 下对 SST 文件进行 level 分级，level 越大 SST 文件越多
      - LSM-tree 下对每层溢出的 SST 文件向下合并
  - LRU 缓存来减少磁盘 SST 文件的读取与写入
  - MEM-Table(跳表) 作为 LRU 尾部节点弹出与刷盘之间的缓冲区
  - 对 LRU 的存放为以内存利用率为主的内存池（连续分区存储管理），支持
      - 空闲节点的切割与合并
      - 空闲链表动态插入节点
      - LRU 内 {键: lru节点} 的映射容器为手写的 Trie
  - WAL 持久化命令，保证数据安全性
- 通信
  - 任意类型数据传输
      - 数据拆分成多个字节发送
      - 收取多个字节后按类型需要的字节数进行合并
  - 主从 Reactor 分发模式下的 Epoll:ET 触发
      - 每个客户端经主Reactor监听连接后平均分配进一个subReactor中响应事件
      - 响应事件的方法为每个客户端只会在一个线程内进行 Epoll:ET 触发的 IO 多路复用
  - `socket` 进程通信
      - 通过字符串通信模拟报头，完成前置指令
      - 通过 `uint8_t` 传输完成字节信息传递（两个主机之间以大端字节序传输）
- 权限与用户
  - `mysql` 连接下的用户登入信息判断
  - 手写容器模板-字典树用于存放 “[用户名] - 数据库对象”
      - 支持 string 与任意类型组成的键值对的存储
      - 支持部分 map<string, 任意类型> 的操作

## 结构

### 整体框架

![](https://cr-demo-blog-1308117710.cos.ap-nanjing.myqcloud.com/chivas-regal/cloudCodingDB-all.svg)

### SST 文件结构

![cloudCodingDB-sst-file](https://cr-demo-blog-1308117710.cos.ap-nanjing.myqcloud.com/chivas-regal/cloudCodingDB-sst-file.svg)


<h1 align="center">服务端</h1>

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

## 客户端接入

本服务端客户端通信均以 `send/recv` 方式交互
下面均是客户端应做的事情

### 通信内容

- 连接上
- 传入一个字符串代表登入用户的**账号**与**密码**
- 传入一个字符串包含**操作字符**与**操作内容**
- **操作字符**
    - `NEW`：表示新建空间，传入**变量名**与**所需要的字节数**
    - `GET`：表示读出数据，传入**变量名**，会**返回一个 `uint_8` 类型的序列**
    - `SET`：表示写入数据，传入**变量名**，再**传入一个 `uint_8` 类型的序列**
    - `DES`：表示释放空间，传入**变量名**
    - 其他：表示程序结束
    
连接 与 登录 与 所有非“其他”的操作，都会在结束后收到一个 **"OK" 字符串**

### 通信格式

对于所有的字符串类通信  
先传入 '#'+单词个数n ，然后是 n 个 '$'+单词长度+' '+单词  
每一部分后面都要带上 "\r\n"  
  
CS对话示例如下：

```
== connect successfully ==
server> OK

client> #2\r\n
        $3 zyz\r\n
        $8 12345678\r\n

(解说：客户端登录，账号=zyz，密码=12345678)

server> OK

client> #3\r\n
        $3 NEW\r\n
        $3 var\r\n
        $1 8\r\n

(解说：客户端新建变量，名为'var'，8个字节，如本意为在客户端有 int* var = new int[2])

server> OK

client> #4\r\n
        $3 SET\r\n
        $3 var\r\n
        $1 4\r\n
        $1 7\r\n
client> 1,3,2,4

(解说：客户端设置 var[1] = 1*8+3*4+2*2+4 )

server> OK

client> #4\r\n
        $3 GET\r\n
        $3 var\r\n
        $1 4\r\n
        $1 7\r\n

(解说：客户端获取 var[1])

server> 1,3,2,4
server> OK

client> #2\r\n
        $3 DES\r\n
        $4 var\r\n

(解说：客户端释放变量 var，delete[] var)

server> OK
```

<h1 align="center">客户端</h1>

## 使用说明

核心逻辑与服务端内的类似，这里只说明类 `Connector` 的使用方法

`Connector` 内部存在几个函数:
- 构造函数：构造的时候完成连接
- 登录函数 `Login()` ，参数为 (string)用户名 ，(string)密码
- 内存申请 `memAsk()` ，有一个模板参数表示申请的变量类型，函数参数 (string)变量名，(int)变量数量
- 变量赋值 `memWrite()` ，模板参数为对应变量类型，函数参数 (string)变量名，(变量类型)变量值，(int)变量位置（非数组0）
- 变量获取 `memRead()`，模板参数为对应变量类型，函数参数 (string)变量名，(int)变量位置（非数组0）
- 内存释放 `memFree()`，函数参数 (string)变量名
- 关闭连接 `Exit()`

配置信息在 "/include/config.h" 中，内部只有两个
    - `SERVER_IP` 服务端 ip 地址
    - `SERVER_PORT` 服务端端口号
