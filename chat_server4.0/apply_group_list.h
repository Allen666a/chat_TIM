#ifndef APPLY_GROUP_LIST_H
#define APPLY_GROUP_LIST_H
#include <vector>
#include <iostream>
using namespace std;
class Apply_group_list{
public:
	vector<string>  user_receive_id;
	vector<string>  user_send_id;
	vector<string>	flg;
	vector<string> group_name;
	Apply_group_list();
	~Apply_group_list();
};
#endif

