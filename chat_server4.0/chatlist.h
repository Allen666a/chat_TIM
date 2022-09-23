#ifndef CHARTINFO_H
#define CHARTINFO_H
#include <event.h>
#include <list>
#include <vector>
#include "GroupClass.h"
#include "chat_database.h"
#include <map>
//#include "Chat_online_member_class.h"
using namespace std;

#define MAXNUM 1024  //表示群的最大数量
class ChatInfo{

private:
	//Chat_online_member_class *online_user;  //保存所有在线用户的信息
	//GroupClass *group_info;  //保存所有群的信息
	//ChatDataBase *chatdatabase;//数据库对象
public:
		
	ChatDataBase *chatdatabase;//数据库对象
	GroupClass *group_info;  //保存所有群的信息
	map<string,struct bufferevent*> online_user;
	ChatInfo();
	~ChatInfo();
	
};
#endif
