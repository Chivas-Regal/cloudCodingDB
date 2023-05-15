<h1 align="center">客户端</h1> 

## 文件目录

```
├── CMakeLists.txt
├── README.md
├── build
... 生成文件，不再放出来了
├── example
│   └── main.cpp
├── include
│   ├── config.h
│   ├── connector.h
│   ├── inet_address.h
│   ├── limits.h
│   ├── socket.h
│   └── util.h
├── lib
│   └── libmem_socket_client.so
└── src
    ├── connector.cpp
    ├── inet_address.cpp
    ├── socket.cpp
    └── util.cpp
```

## 使用说明

核心逻辑与服务端内的类似，这里只说明类 `Connector` 的使用方法

`Connector` 内部存在几个函数
    - 构造函数：构造的时候完成连接
    - 登录函数 `Login()` ，参数为 (string)用户名 ，(string)密码
    - 内存申请 `memAsk()` ，有一个模板参数表示申请的变量类型，函数参数 (string)变量名，(int)变量数量
    - 变量赋值 `memWrite()` ，模板参数为对应变量类型，函数参数 (string)变量名，(变量类型)变量值，(int)变量位置（非数组0）
    - 变量获取 `memRead()`，模板参数为对应变量类型，函数参数 (string)变量名，(int)变量位置（非数组0）
    - 内存释放 `memFree()`，函数参数 (string)变量名
    - 关闭连接 `Exit()`