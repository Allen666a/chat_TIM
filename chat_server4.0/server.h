#ifndef SERVER_H  //防止编译时冲突
#define SERVER_H
#include <event.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread> 
#include <iostream>
#include <event2/listener.h>
#include "chatlist.h"
#include "apply_list.h"
#include "apply_group_list.h"
#include <jsoncpp/json/json.h>
#include <vector>
#include <iostream>
#include <unistd.h>
using namespace std;
#define IP "10.0.24.15" //服务器的内网ip
#define PORT 9000
#define MAXSIZE  1024 * 1024
class Server{

private:
	struct event_base *base; //事件集合
	struct evconnlistener *listener ; //监听事件 包含了bind sockt listen accpet..
	static ChatInfo *chatlist; //链表对象 含有两个链表-目前待优化    | 已经优化重构了数据结构不在用链表操作
	static ChatDataBase *chatdb;//数据库对象
private:
	//回调函数
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,struct sockaddr *addr, int socklen, void *arg); //evconnlistener_new_bind的静态回调函数用于处理连接的客户端
	static void client_handler(int);	
	static void send_file_handler(int,int,int*,int*);
	static void read_cb(struct bufferevent *bev, void *ctx);
	static void event_cb(struct bufferevent *bev, short what, void *ctx);
	static void server_register(struct bufferevent* bev,Json::Value val);
	static void server_login(struct bufferevent* bev,Json::Value val);
	static void server_add_friend(struct bufferevent* bev,Json::Value val);	
	static void server_create_group(struct bufferevent* bev,Json::Value val);
	static void server_deal_friend_apply(struct bufferevent* bev,Json::Value val);	
	static void server_add_group(struct bufferevent* bev,Json::Value val);	
	static void server_group_appply_deal(struct bufferevent* bev,Json::Value val);	
	static void server_private_chat(struct bufferevent* bev,Json::Value val);		
	static void server_group_chat(struct bufferevent* bev,Json::Value val);	
	static void server_get_group_member(struct bufferevent* bev,Json::Value val);	
	static void server_user_offline(struct bufferevent* bev,Json::Value val);	
	static void server_send_file(struct bufferevent* bev,Json::Value val);	
	static void server_get_apply_info(struct bufferevent* bev,Json::Value val);	
	
public :
	//封住封包函数
	static char * get_data_packet(string,int&);
	Server(const char *ip="127.0.0.1",int port=9000);
	~Server();
};

#endif
