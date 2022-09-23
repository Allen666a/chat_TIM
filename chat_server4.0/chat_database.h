#ifndef CHART_DATABASE_H
#define CHART_DATABASE_H

#include <mysql/mysql.h>
#include <iostream>
#include <stdio.h>
#include "GroupClass.h"
#include <vector>
#include "apply_list.h"
#include "apply_group_list.h"
#include <jsoncpp/json/json.h>
using namespace std;


class ChatDataBase{
private:
	MYSQL *mysql;
public:
	ChatDataBase();
	~ChatDataBase();
	
	void my_database_connect(const char*); //数据库连接
	void my_database_get_group_info(GroupClass*,string);//得到的一系列信息都存储到类中  参数是传出参数
	bool my_database_form_info_exist(string,string); //公共函数，查询某个表的某个记录是否存在 存在返回 true 不存在 false 查的是user_id
	void my_database_add_user(string form,string user_id,string user_password);
	bool my_dadtabase_chek_user_password(string,string); //验证成功 返回 true 否则 false
	bool my_database_is_friend(string,string);//判断是否已经是好友关系 是好友返回true，不是好友返回false
	bool my_database_is_already_apply(string,string);//判断用户之前是否已经提出申请并且是未处理状态，如果有这个记录并且是未处理返回false，如果没有这个记录，或者是拒绝状态返回true
	void my_database_fix_apply_list(string,string);//修改apply_list表
	string my_databases_get_friends(string,string); //表和id
	bool my_database_form_info_exist(string,string,string);//重载，判断哪个表的哪个用户哪个东西是否存在
	void my_database_disconnect();
	void my_database_add_new_group(string,string);//创建群
	void my_database_get_apply_list(Apply_list*,Apply_group_list*,string);//得到好友申请的信息 传出参数
	void my_database_update_apply_list_info(string,string,string);
	void my_database_append_text(string,string,string,string);//表，字段，内容 追加
	bool my_database_check_exist(string,string,string,string);//通用，检查某个表中的某个字段的某个内容是否存在
	bool my_database_is_already_in_group(string,string);//判断用户是否已经在群里
	bool my_database_is_already_group_apply(string,string,string&);
	void my_database_fix_apply_group_list(string,string,string);//修改apply_group_list 相当于添加咯
	void my_database_get_apply_group_list(Apply_group_list*,string);
	void my_database_update_apply_group_list(string,string,string,string);//修改apply_group_list表，设置同意还是拒绝
	void my_dadtabase_append_group_number_text(string,string);
	void my_database_fix_message_list_form(string,string);
	void my_database_fix_message_list_form(string);
	string  my_databasse_get_chek_message(string);	
	vector<string> Parse_string(string);
	vector<string> my_database_get_group_number(string);
	void my_database_set_message_already(string);
	void my_database_insert_private_recod(Json::Value);
	void my_database_fix_group_user(string,string);
};
#endif
