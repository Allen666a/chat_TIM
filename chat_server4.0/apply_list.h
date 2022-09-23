#ifndef APPLY_LIST_H 
#define APPLY_LIST_H
#include <vector>
#include <iostream>
using namespace std;
class Apply_list{
public:
	vector<string>  user_receive_id;
	vector<string>  user_send_id;
	vector<string>	flg;
	Apply_list();
	void list_clear();
	~Apply_list();
};
#endif
