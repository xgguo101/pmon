## 简介

使用 netlink connector 监控进程示例。

## 安装

仅限 linux 系统.

```shell
cd pmon
make && make install
```

在当前目录会生成可执行文件.


## 使用

1、监听所有事件

```shell
./pmon
```

2、监听指定进程事件

```shell
./pmon -p pid
```
