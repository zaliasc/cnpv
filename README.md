# cnpv

#### 介绍
Container network permission virtualization

#### install

```
cd ns_agent
make all
```

生成`agent.so` 动态库，`LD_PRELOAD`到应用程序

日志、cache相关配置在ns_agent/types.h中修改

```
// logs 日志级别控制
#define LOG_INFO
#define LOG_DEBUG
#define LOG_WARN
#define LOG_ERR
#define LOG_FATAL

#define DEBUG_FILE //打印日志到文件
// #define DEBUG_STDOUT //打印日志到stdout 和 打印到文件 不能同时选择

// 是否开启cache
#define USE_CACHE
```



#### 配置文件

```
{
  "app-test":[
    {"PATH": "/home/zhuzhicheng/project/cnpv/perf_test/test.c", "AUTHORITY": "wr"},
    {"PATH": "/home/zhuzhicheng/project/cnpv/perf_test/flame.sh", "AUTHORITY": "wr"}
  ],
  "nginx":[
    {"PATH": "/home/zhuzhicheng/project/nginx_file/index.html", "AUTHORITY": "wr"},
    {"PATH": "/home/zhuzhicheng/project/nginx_file/js/", "AUTHORITY": "F"}
  ]
}
```

在config.json中进行**文件/目录**访问权限限制 (如果是目录 以`/`结尾) 

包含权限有R(read)/W(write)/F(forbidden)

一个配置文件中可以书写多个可执行程序的访问权限限制

#### 使用方式

```
#以配合nginx使用为例
#现在配置文件中配置好 和 nginx 相关的文件限制，再如下方式启动nginx
CNPV_PATH=$PWD/config.json LD_PRELOAD=$PWD/agent.so nginx
#生成的日志在当前目录下 命名格式为cnpv-nginx.log
```