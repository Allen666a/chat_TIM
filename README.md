# chat_TIM

项目基于 C/S 架构，实现了用户注册、登录、添加好友、创建群、聊天、群发、文件 传输以及上下线提醒功能。服务端部署在腾讯云服务器上，使用 libevent + 多线程来解决 并发的问题，可以同时处理多个客户端的数据信息。文件传输单独启用了文件服务器，实现 数据 的转发。客户端使用 Qt，通过 json 格式完成跟服务器的数据交互。

<br>

界面很丑，没用到qml
