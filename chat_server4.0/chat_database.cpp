#include "chat_database.h"

ChatDataBase::ChatDataBase(){
}
ChatDataBase::~ChatDataBase(){
	mysql_close(mysql);
}
void ChatDataBase::my_database_connect(const char* DatabaseName){		
	mysql=NULL;
	mysql=mysql_init(NULL);//会申请一个mysql对象给mysql
	mysql=mysql_real_connect(mysql,"localhost","allen","123456",DatabaseName,0,NULL,0);//因为程序要跑在服务器上，mysql也在服务器上所以用本地就行
	if(NULL==mysql){
		cout<<"chat_database.cpp 12 line connect databases failure!"<<endl;
	}
}

void ChatDataBase::my_database_get_group_info(GroupClass *groupinfo,string user_id){
	//问题 当我登录上后 在修改我的网名 修改后也要改变数据库的名字也要改群主或群成员名字  未处理  2022/7/31
//	if(mysql_query(mysql,"set names utf8;")!=0){
//		cout<<"chat_database.cpp 19 mysql_query error"<<endl;
//	}
	string str="select * from group_user where user_id='"+user_id+"';";
	cout<<str<<endl;
	const char *sql=str.c_str();
		

	if(mysql_query(mysql,sql)!=0){ //待修改 2022/8/8  出现问题，两次登录出错 
		cout<<"chat_database.cpp 22 mysql_query error"<<endl;
	}
	
	cout<<groupinfo->group_num<<"看看是不是静态对象的问题"<<endl;
	
	MYSQL_RES *res=mysql_store_result(mysql);
	if(NULL==res){
		cout<<"chat_database.cpp 27 mysql_store_result error!!"<<endl;
	}
	
	
	//如果群的拥有者和自己的user_id一样,说明自己就是群主
	MYSQL_ROW row;
	int group_count=0,group_member_count=0;
	string owner="";
	str="";
	vector<string> tmp;
	
	//需要大修改这个模块
	while(row=mysql_fetch_row(res)){
		group_count++;	
		//if(row[0]!=NULL)
			groupinfo->group_name.push_back(row[1]);
		//else{	
		//	group_count--;
		//	continue;
		//	groupinfo->group_name.push_back("null");
			
		//}	
	//	if(row[1]!=NULL){
			groupinfo->group_ower.push_back(row[2]);
			owner=row[2];
	//	}else{	
	//		groupinfo->group_ower.push_back("");
	//		owner="null";
	//	}
		//groupinfo->group_ower.push_back(row[1]);
	//	owner=row[1];
	//	if(row[2]!=NULL)
			//str=row[2];
	//	else
	//		str="";
	str=row[1];	
	string str2="select * from group_info where group_name='"+str+"';";
	const char *sql2=str2.c_str();
		

	if(mysql_query(mysql,sql2)!=0){ 
		cout<<"chat_database.cpp 77 mysql_query error"<<endl;
	}
		
	MYSQL_RES *res2=mysql_store_result(mysql);
	MYSQL_ROW  row2;
	
	row2=mysql_fetch_row(res2);
	str=row2[1];
		//解析成员信息
		int start=0;
		int end=0; 
		while(1){
			end=str.find('|',start);		
			if(end==-1){
				group_member_count++;
				groupinfo->group_member_num.push_back(group_member_count);
				group_member_count=0;
				//if(str.substr(start,end-start)!="")
				tmp.push_back(str.substr(start,end-start));
				groupinfo->group_member.push_back(tmp);
				tmp.clear();
				//	groupinfo->group_member.push_back(str.substr(start,end-start));
				//else	
				//	groupinfo->group_member[owner].push_back("null");
				break;
			}
			
			tmp.push_back(str.substr(start,end-start));
		//	if(str.substr(start,end-start)!="")
		//		groupinfo->group_member[owner].push_back(str.substr(start,end-start));
		//	else	
		//		groupinfo->group_member[owner].push_back("null");
			start=end+1;
			group_member_count++;
		}
			
	};
	groupinfo->group_num=group_count;
	//cout<<"测试测试测试"<<endl;
}

bool ChatDataBase::my_database_form_info_exist(string form,string username){
	//if(mysql_query(mysql,"set  chat_mysql  utf8;")!=0){
	//	cout<<"chat_database.cpp 67 mysql_query is error!!"<<endl;
	
	//}
	string s="select user_id from "+form+" where user_id="+"'"+username+"'"+";";
//	cout<<s<<endl;
	const char* sql=s.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"  chat_database.cpp 73  mysql_query error1"<<endl;
	}
	MYSQL_RES *res=mysql_store_result(mysql);
	if(res==NULL){
		cout<<"chat_database.cpp 77 line  mysql_store_result error!"<<endl;
	}
	MYSQL_ROW row=mysql_fetch_row(res);
	if(NULL==row){//用户不存在可以注册
		return false;
	}else{
		return true;
	}
}
bool ChatDataBase::my_database_form_info_exist(string form,string user_id,string info){
	string str="select * from "+form+" where  group_name='"+info+"';";
	const char*sql=str.c_str();
	
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_database.cpp on line"<<__LINE__<<endl;
	}
	MYSQL_RES *res=mysql_store_result(mysql);
	if(res==NULL){	
		cout<<"chat_database.cpp on line"<<__LINE__<<endl;
	}
	MYSQL_ROW row=mysql_fetch_row(res);
	if(NULL==row){
		return false;
	}
	return true;
}
void ChatDataBase::my_database_add_user(string form,string user_id,string user_password){
	string str="insert into "+form+" values  "+"("+"'"+user_id+"',"+"'"+user_password+"'"+")"+";";
	//cout<<str<<endl;
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp mysql_query error on line 125 my_database_add_user"<<endl;
	}
	str="insert into message_list_info values('"+user_id+"','0');";
	sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp mysql_query error on line "<<__LINE__<<" my_database_add_user"<<endl;
	}

	str="insert into friend_relationship(user_id,user_friend) values('"+user_id+"','"+user_id+"');";
	sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp mysql_query error on line "<<__LINE__<<" my_database_add_user"<<endl;
	}
}
bool ChatDataBase::my_dadtabase_chek_user_password(string user_id,string user_password){
	string str="select * from user_register where user_id='"+user_id+"'"+" and  user_password='"+user_password+"'"+";";	
	cout<<str<<"检查用户密码"<<endl;
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line  133 my_database_add_user"<<endl;
	}
	
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	if(NULL==row){ //说明密码错误
		return false; 
	}
	return true;
}
//返回好友列表
string ChatDataBase::my_databases_get_friends(string form,string user_id){
	string str="select user_friend from "+form+" where "+"user_id='"+user_id+"';";	
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line  148 my_database_add_user"<<endl;
	}
	cout<<str<<endl;
	string result="";
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	if(row==NULL || row[0]==NULL){ //没有好友
		cout<<"没有好友的"<<endl;
		return result;
	}else{	
		cout<<"cssasaasas"<<endl;
		result.append(row[0]);
		return result;
	}
}
bool ChatDataBase::my_database_is_friend(string user_id,string friend_id){
	string str="select user_friend  from friend_relationship where  user_id='"+user_id+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line  165 my_database_add_user"<<endl;
	}
	cout<<str<<endl;
	string result="";
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	vector<string> friend_list=Parse_string(row[0]);
	for(int i=0;i<friend_list.size();i++){
		if(friend_list[i]==friend_id){
			return true;
		}
	}	
	return false;
}
vector<string> ChatDataBase::Parse_string(string strinfo){
	vector<string> res;
	
	int start=0,end=0;
	while(1){
		 end=strinfo.find('|',start);
		if(end==-1){
			res.push_back(strinfo.substr(start,end-start));
			break;
		}
		res.push_back(strinfo.substr(start,end-start));
		start=end+1;
	}

	return res;
}
bool ChatDataBase::my_database_is_already_apply(string user_id,string friend_id){
	string str="select * from apply_list where user_receive_id='"+friend_id+"'"+" and user_send_id='"+user_id+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line  199 my_database_add_user"<<endl;
	}
	cout<<str<<endl;
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	if(row==NULL || row[2]=="0"){
		return true;
	}
	return false;
}
void ChatDataBase::my_database_fix_apply_list(string user_id,string friend_id){
	string str="insert into apply_list values('"+friend_id+"','"+user_id+"',2);";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line  212 my_database_add_user"<<endl;
	}
	cout<<str<<endl;
}
void ChatDataBase::my_database_add_new_group(string user_id,string group_name){
	string str="insert into group_info  values('"+group_name+"','"+user_id +"');";
	
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp mysql_query error on line "<<__LINE__<<" my_database_add_user"<<endl;
	}

	str="insert into group_user values('"+user_id+"','"+group_name+"','"+user_id+"');";
	sql=str.c_str();
	
	if(mysql_query(mysql,sql)!=0){		
		cout<<"chat_databases.cpp mysql_query error on line "<<__LINE__<<" my_database_add_user"<<endl;
	}
}
void ChatDataBase::my_database_get_apply_list(Apply_list *applist ,Apply_group_list *apply_group_list,string user_id){
	string str="select * from apply_list where user_receive_id='"+user_id+"' or user_send_id='"+user_id+"' order by flg desc;";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	cout<<str<<endl;
		
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row;
	while(row=mysql_fetch_row(res)){
		applist->user_receive_id.push_back(row[0]);
		applist->user_send_id.push_back(row[1]);
		applist->flg.push_back(row[2]);
	}


	str="select * from apply_group_list  where user_receive_id='"+user_id+"' or user_send_id='"+user_id+"' order by flg desc;";
	sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	cout<<str<<endl;
		
	MYSQL_RES *res2=mysql_store_result(mysql);
	MYSQL_ROW row2;
	while(row2=mysql_fetch_row(res2)){
		apply_group_list->user_receive_id.push_back(row2[0]);
		apply_group_list->user_send_id.push_back(row2[1]);
		apply_group_list->flg.push_back(row2[2]);
		apply_group_list->group_name.push_back(row2[3]);
	}

}
void ChatDataBase::my_database_update_apply_list_info(string send_id,string receive_id,string flg){
	string str="update apply_list set flg="+flg+" where user_send_id='"+send_id+"' and user_receive_id='"+receive_id+"';";
	cout<<str<<endl;
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases  on line"<<__LINE__<<endl;
	}
	//显示已阅读
	str="update message_list_info set chek_message=0 where user_id='"+send_id+"';";
	sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases  on line"<<__LINE__<<endl;
	}
}
void  ChatDataBase::my_database_append_text(string form ,string field,string info,string user_id){
	string str="update "+form+" set "+field+"=CONCAT("+field+",'"+info+"') where user_id='"+user_id+"';";	
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases  on line"<<__LINE__<<endl;
	}
}
bool ChatDataBase::my_database_check_exist(string form,string field,string filed2,string info){

	string s="select "+ field +" from "+form+" where "+filed2+"='"+info+"';";
	cout<<s<<endl;
	const char* sql=s.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"  chat_database.cpp 73  mysql_query error1"<<endl;
	}
	MYSQL_RES *res=mysql_store_result(mysql);
	if(res==NULL){
		cout<<"chat_database.cpp 77 line  mysql_store_result error!"<<endl;
	}
	MYSQL_ROW row=mysql_fetch_row(res);
	if(row==NULL){
		return false;
	}
	return true;
}
bool ChatDataBase::my_database_is_already_in_group(string user_id,string group_name){
	string str="select * from group_info where group_name='"+group_name+"';";
	
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
		
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	str=row[1];
	vector<string> tmp=Parse_string(str);
	for(int i=0;i<tmp.size();i++){
		if(user_id==tmp[i]){
			return true;
		}
	}
	return false;
}
bool ChatDataBase::my_database_is_already_group_apply(string user_id,string group_name,string &group_owner){
	string str="select * from group_user where group_name='"+group_name+"';";
	cout<<str<<endl;
	const char*sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	string tmp=row[2];
	group_owner=tmp;	
	str="select * from apply_group_list where user_receive_id='"+tmp+"' and user_send_id='"+user_id+"' and flg=2 and group_name='"+group_name+"';";
	sql=str.c_str();

	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	res=mysql_store_result(mysql);
	row=mysql_fetch_row(res);
	if(row==NULL){
		return false;
	}

	return  true;
}
void ChatDataBase::my_database_fix_apply_group_list(string recive,string send,string group_name){
	string str="insert into apply_group_list values('"+recive+"','"+send+"',2,'"+group_name+"');";
	cout<<str<<endl;
	const char*sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}
void ChatDataBase::my_database_get_apply_group_list(Apply_group_list* apply_list,string user_id){
		
	string str="select * from  apply_group_list  where user_receive_id='"+user_id+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	cout<<str<<endl;
		
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row;
	while(row=mysql_fetch_row(res)){
		apply_list->user_receive_id.push_back(row[0]);
		apply_list->user_send_id.push_back(row[1]);
		apply_list->flg.push_back(row[2]);
		apply_list->group_name.push_back(row[3]);
	}
	
}
void ChatDataBase::my_database_update_apply_group_list(string send_id,string receive_id,string flg,string group_name){
	
	string str="update apply_group_list set flg="+flg+" where user_send_id='"+send_id+"' and user_receive_id='"+receive_id+"' and group_name='"+group_name+"';";
	cout<<str<<endl;
	const char* sql=str.c_str();
	if(mysql_query(mysql,sql)!=0){
		cout<<"chat_databases  on line"<<__LINE__<<endl;
	}
}
void ChatDataBase::my_dadtabase_append_group_number_text(string group_name,string group_number){
	string tm="|";
	tm+=group_number;
	string str="update group_info  set group_number=CONCAT(group_number,'"+tm+"') where group_name='"+group_name+"';";	
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}

}
void ChatDataBase::my_database_fix_group_user(string group_name,string user){


	string str="select group_owner  from group_user where group_name='"+group_name+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	str=row[0];

	string s="insert into group_user values('"+user+"','"+group_name+"','"+str+"');";
	sql=s.c_str();
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}
vector<string> ChatDataBase::my_database_get_group_number(string group_name){
	string str="select group_number  from group_info where group_name='"+group_name+"';";
	
	cout<<str<<endl;	
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	str=row[0];
	vector<string> v=Parse_string(str);	
	return v;
}
string  ChatDataBase::my_databasse_get_chek_message(string user_id){
	string str="select chek_message from message_list_info where user_id='"+user_id+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
	MYSQL_RES *res=mysql_store_result(mysql);
	MYSQL_ROW row=mysql_fetch_row(res);
	str=row[0];
	return str;
}
void ChatDataBase::my_database_fix_message_list_form(string id1,string id2){

	string str="update message_list_info set chek_message=1 where user_id='"+id1+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}

	str="update message_list_info set chek_message=1 where user_id='"+id2+"';";
	sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}

void ChatDataBase::my_database_fix_message_list_form(string id1){

	string str="update message_list_info set chek_message=1 where user_id='"+id1+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}
void ChatDataBase::my_database_set_message_already(string user_id){
	//修改message_list表中的flg=0 已读
	string str="update message_list_info set chek_message=0 where user_id='"+user_id+"';";
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}
void ChatDataBase::my_database_insert_private_recod(Json::Value v){

	string  s=Json::FastWriter().write(v);
	string str="insert into chat_records(record,mdate) values('"+s+"',now());";
	cout<<str<<endl;	
	const char* sql=str.c_str();	
	if(mysql_query(mysql,sql)!=0){	
		cout<<"chat_databases.cpp on line"<<__LINE__<<endl;
	}
}
void ChatDataBase::my_database_disconnect(){
	mysql_close(mysql);	
}

