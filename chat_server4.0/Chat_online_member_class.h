#ifndef CHART_ONLINE_H
#define CHART_ONLINE_H
#include <vector>
#include <iostream>
using namespace std;
class Chat_online_member_class{
public:
	//struct bufferevent *bev;
	vector<string> name;//在线成员的名字
	Chat_online_member_class();
	~Chat_online_member_class();
};

#endif
