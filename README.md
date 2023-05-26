<h1 align="center">云端存储型编程向数据仓库</h1>

## 作用

网络云空间很多，主体为支持普通用户的类似于云盘、云点播这类存放固定内容的平台  
本工程可以用来搭建一个**支持开发人员存放数据的网络远程空间**  
我们可以把 server 端搭在内存充足网络带宽速率快的服务器上，就可以按下面 client 演示在各个客户端使用这个服务器了

## 使用演示

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
conn->Login("test1", "test1");     // 登录已经注册在服务端的用户 (test1, test1)
conn->memAsk<node>("hello", 100);  // 申请数组 hello[100]
for (int i = 0; i < 10; i ++)      // 给每个 hello[i] = node(i, i + 1)
    conn->memWrite<node>("hello", node(i, i + 1), i);  
for (int i = 0; i < 10; i ++) {    // 输出每个 hello[i].x，hello[i].y
    node rd = conn->memRead<node>("hello", i);
    std::cout << "->>: " << rd.x << " " << rd.y << std::endl;
}
conn->memFree("hello"); // 释放 hello 的空间
conn->Exit();           // 结束
```

当然运行之前必须要把 server 端打开运行  
会输出：

```
->> 0 1
->> 1 2
->> 2 3
->> 3 4
->> 4 5
->> 5 6
->> 6 7
->> 7 8
->> 8 9
->> 9 10
```

这里我们写好的数值都可以在后面利用到来完成编程向数据库  
**注意这里数据的申请是不费我们自己的内存的**
