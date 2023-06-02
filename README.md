<h1 align="center">云端存储型编程向数据仓库</h1>

这里只给出了最基本的使用说明，**细节操作与技术介绍请分别进入工程内两份代码目录的 README.md 进行阅览**

## 作用

网络云空间很多，主体为支持普通用户的类似于云盘、云点播这类存放固定内容的平台  
本工程可以用来搭建一个**支持开发人员存放数据的网络远程空间**  
我们可以把 server 端搭在内存充足网络带宽速率快的服务器上，就可以按下面 client 演示在各个客户端使用这个服务器了

## code-api 使用演示

类似在 mem_socket_client 端写的：

```cpp
// 一个自定义结构体
struct node {
    int x, y;
    node (int _x = 0, int _y = 0) :
        x(_x),
        y(_y) {}
};

Connector *conn = new Connector(); // 初始化一个 tcp 连接
conn->Login("test1", "test1"); // 登录 usr: test1, pwd: test1
conn->memAsk<node>("hello", 100); // 新建 node 数组 hello[100]

// 对每一个 hello[i] 赋值 {i, i + 1}
for (int i = 0; i < 10; i ++) {
    conn->memWrite<node>("hello", node(i, i + 1), i);
    std::cout << "client> SET hello[" << i << "] = {" << i << ", " << i + 1 << "}\n";
}

// 输出每一个 hello[i] 的值
for (int i = 0; i < 10; i ++) {
    node rd = conn->memRead<node>("hello", i);
    std::cout << "client> GET hello[" << i << "]\n";
    std::cout << "server> {" << rd.x << ", " << rd.y << "}\n";
}

// 释放 node 数组 hello
conn->memFree("hello" + std::to_string(id));
// 关闭连接
conn->Exit();
exit(0);
```

当然运行之前必须要把 server 端打开运行  
输出如下：  

![codeCloudDB-code-api-show](https://cr-demo-blog-1308117710.cos.ap-nanjing.myqcloud.com/chivas-regal/codeCloudDB-code-api-show.gif)

## local-cmd 

这里就是我们本地运行时直接使用命令完成 KV 操作  

![codeCloudDB-localcmd-show](https://cr-demo-blog-1308117710.cos.ap-nanjing.myqcloud.com/chivas-regal/codeCloudDB-localcmd-show.gif)

这里我们写好的数值都可以在后面利用到来完成编程向数据库  
**注意这里数据的申请是不费我们自己的内存的**
