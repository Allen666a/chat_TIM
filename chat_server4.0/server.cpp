#include "server.h"

ChatDataBase*  Server::chatdb=new ChatDataBase();
ChatInfo*  Server::chatlist=new ChatInfo();
Server::Server(const char *ip,int port){
	
//	chatlist=new ChatInfo(); //new 链表对象 （链表对象获取数据库的信息，链表对象又包含封装的数据库类）
		
	//创建集合
	base=event_base_new();

	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=inet_addr(ip);	

	//创建监听对象
	listener=evconnlistener_new_bind(base,listener_cb,NULL,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,10,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(NULL==listener){
		cout<<"server.cpp 20 line  evconnlistener_new_bind error!!!"<<endl;
	}
	
	cout<<"服务器初始化成功！开始监听客户端的连接"<<endl;
	event_base_dispatch(base); //监听集合	
	event_base_free(base); //释放集合
}
//当有客户端发起连接的时候，会触发该函数
void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,struct sockaddr *addr, int socklen, void *arg){
	//有客户端连接了
	cout<<"接受客户端的连接，fd="<<fd<<endl;

	//创建线程来处理该客户端
	thread client_thread(client_handler,fd); //c++11 的一个thread类，构造函数
	client_thread.detach(); //线程分离 不用回收了，线程运行结束了自动回收资源
}
void Server::client_handler(int fd){
	
	//创建集合
	struct event_base *base=event_base_new();
	//创建bufferevent对象  不用在操作文件描述符了
	struct bufferevent *bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
	if(NULL==bev){
		cout<<"server.cpp 43 line bufferevent_sockte_new error!!"<<endl;
	}
	//给bufferevent设置回调函数
	bufferevent_setcb(bev,read_cb,NULL,event_cb,NULL);
	
	//使能回调函数
	bufferevent_enable(bev,EV_READ);

	event_base_dispatch(base);//监听集合 客户端是否有数据发生过来
	event_base_free(base); //释放集合
}

void Server::read_cb(struct bufferevent *bev, void *ctx){
	char buf[1024]={0};
	int size=bufferevent_read(bev,buf,sizeof(buf));
	if(size<0){
		cout<<"server.cpp 59 line bufferevent_read error !"<<endl;
	}	
	cout<<buf<<endl;
	
	//下面是业务处理逻辑
	
	//jsoncpp解析
	//
	Json::Reader readers;//解析json
	Json::FastWriter writer;//封装json
	Json::Value val;
	if(!readers.parse(buf,val)){ //把字符串解析成json对象
		cout<<"server.cpp 71 line 服务器解析客户端json字符串失败"<<endl;
	}
	string cmd=val["cmd"].asString(); //json对象转换成字符串
	cout<<cmd<<endl;
	if(cmd=="register"){ //注册功能
		server_register(bev,val);
	}else if(cmd=="login"){//登录功能
		server_login(bev,val);
	}else if(cmd=="add"){//添加好友功能
		server_add_friend(bev,val);
	}else if(cmd=="add_apply_deal"){
		server_deal_friend_apply(bev,val);
	}else if(cmd=="create_group"){
		server_create_group(bev,val);
	}else if(cmd=="add_group"){
		server_add_group(bev,val);
	}else if(cmd=="apply_group_deal"){
		server_group_appply_deal(bev,val);
	}else  if(cmd=="private_chat"){
		server_private_chat(bev,val);
	}else if(cmd=="group_chat"){
		server_group_chat(bev,val);	
	}else if(cmd=="get_group_member"){//
		server_get_group_member(bev,val);
	}else if(cmd=="offline"){
		server_user_offline(bev,val);
	}else if(cmd=="send_file"){
		server_send_file(bev,val);	
	}else if(cmd=="get_apply_list_info"){
		server_get_apply_info(bev,val);	
	} 
	
	
}
void Server::event_cb(struct bufferevent *bev, short what, void *ctx){
	
	chatdb->my_database_connect("chat_mysql2");
	cout<<"客户端下线"<<endl;
	string user_id;
	for(auto it : chatlist->online_user){
		if(it.second==bev){
			cout<<"下线YES"<<endl;
			user_id=it.first;	
			break;
		}
	}
	cout<<"下线的id:"<<user_id<<endl;
	cout<<"目前在线用户"<<endl;
	for(auto it : chatlist->online_user){
		cout << it.first<<endl;
	}
	chatlist->online_user.erase(user_id);
	cout<<"下线后目前在线id"<<endl;	
	for(auto it : chatlist->online_user){
		cout << it.first<<endl;
	}
	//给他的所有在线好友发送离线信息	
	string frineds_list2=chatdb->my_databases_get_friends("friend_relationship",user_id);
	vector<string> friensv2=chatdb->Parse_string(frineds_list2);
	Json::Value v;	
	string str="";
	for(int i=0;i<friensv2.size();i++){
		if(chatlist->online_user.count(friensv2[i])){
			cout<<"用户的好友在线"<<friensv2[i]<<endl;					
			struct  bufferevent*bv=chatlist->online_user[friensv2[i]];
			v["cmd"]="friend_offline";
			v["user_id"]=user_id;
			str=Json::FastWriter().write(v);
			int len=0;	
			char* data=get_data_packet(str,len);
			if(bufferevent_write(bv,data,len)<0){//回写客户端
				cout<<"server.cpp on line"<<__LINE__<<endl;
			}
			free(data);
		}
	}	
	chatdb->my_database_disconnect();
}
/*
void Server::server_get_apply_list(struct bufferevent*bev,Json::Value val){	
	chatdb->my_database_connect("chat_mysql2");
	Apply_list *apply_info=new Apply_list();
	chatdb->my_database_get_apply_list(apply_info,val["user_id"].asString());	
	
		Json::Value vl;
		vl["cmd"]="add_new_friend_info";
	for(int i=0;i<apply_info->user_receive_id.size();i++){
		vl["result"]["user_receive_id"].append(apply_info->user_receive_id[i]);
		vl["result"]["user_send_id"].append(apply_info->user_send_id[i]);
		vl["result"]["flg"].append(apply_info->flg[i]);
	}	
		string  str=Json::FastWriter().write(vl);
		if(bufferevent_write(bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端
			cout<<"server.cpp on line"<<__LINE__<<endl;
		}
	chatdb->my_database_disconnect();
	delete apply_info;
}*/
void Server::server_register(struct bufferevent*bev,Json::Value val){
	chatdb->my_database_connect("chat_mysql2");
	if(chatdb->my_database_form_info_exist("user_register",val["user_id"].asString())){  //表和这个string类型记录是否存在  
		
		cout<<"用户存在！"<<endl;
		//用户存在
		Json::Value val;
		val["cmd"]="register_reply";
		val["result"]="failure";
		string  str=Json::FastWriter().write(val);
	
			int len=0;	
			char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 91 line  bufferevent_write error"<<endl;
		}
		free(data);
		
	}else{
		chatdb->my_database_add_user("user_register",val["user_id"].asString(),val["user_password"].asString());
		cout<<"用户不存在"<<endl;
		//用户不存在	
		Json::Value val;
		val["cmd"]="register_reply";
		val["result"]="success";
		string  str=Json::FastWriter().write(val);
	
		int len=strlen(str.c_str());
		char *data=(char*)malloc(len+4);
		int biglen=htonl(len);
		memcpy(data,&biglen,4);
		memcpy(data+4, str.c_str(), len);

		if(bufferevent_write(bev,data,len+4)<0){//回写客户端
			cout<<"server.cpp 102 line  bufferevent_write error"<<endl;
		}
		
		
	}
	chatdb->my_database_disconnect();
}

void Server::server_add_friend(struct bufferevent*bev,Json::Value val){
	chatdb->my_database_connect("chat_mysql2");
	if(!chatdb->my_database_form_info_exist("user_register",val["friend_id"].asString())){//添加的用户不存在
//		cout<<"cacacaccac"<<endl;
		Json::Value vl;
		vl["cmd"]="add_reply";
		vl["result"]="user_not_exist";
		string  str=Json::FastWriter().write(vl);
		
			int len=0;	
			char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 128 line  bufferevent_write error"<<endl;
		}
		free(data);
	}else if(chatdb->my_database_is_friend(val["user_id"].asString(),val["friend_id"].asString())){//添加的用户存在，但是已经是好友

		Json::Value vl;
		vl["cmd"]="add_reply";
		vl["result"]="already_friend";
		string  str=Json::FastWriter().write(vl);
		
			int len=0;	
			char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 137 line  bufferevent_write error"<<endl;
		}

		free(data);
	}else{//添加的用户存在,还不是好友
		if(chatdb->my_database_is_already_apply(val["user_id"].asString(),val["friend_id"].asString())){//防止重复添加操作数据库
			//修改双方的message_list_info表
			chatdb->my_database_fix_message_list_form(val["user_id"].asString(),val["friend_id"].asString());			
			
			//先修改apply_list信息
			chatdb->my_database_fix_apply_list(val["user_id"].asString(),val["friend_id"].asString());
			//判断申请添加好友的id，是否在线，如果在线，通知有人添加直接未好友，查询数据库数据库
			if(chatlist->online_user.count(val["friend_id"].asString())){
				struct bufferevent*bv=chatlist->online_user[val["friend_id"].asString()];
				Json::Value vl;
				vl["cmd"]="add_apply_info";
				string  str=Json::FastWriter().write(vl);

			int len=0;	
			char* data=get_data_packet(str,len);
				if(bufferevent_write(bv,data,len)<0){//回写客户端
	                        	 cout<<"server.cpp on line"<<__LINE__<<endl;
        		         }	
				free(data);
			}		
			//不在线，等用户每次登录成功后都需要查询一遍apply_list，是否有关于自己的信息  、、这里的逻辑应该写在登录成功哪里
		}
		//回复send客户端发送成功		
		Json::Value vl;
		vl["cmd"]="add_reply";
		vl["result"]="add_success";
		string  str=Json::FastWriter().write(vl);
		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	                 cout<<"server.cpp on line"<<__LINE__<<endl;
        	}
	}
}

void Server::server_create_group(struct bufferevent*bev,Json::Value val){
	chatdb->my_database_connect("chat_mysql2");
	if(!chatdb->my_database_form_info_exist("group_info",val["user_id"].asString(),val["group_name"].asString())){
		//没找到则可以建群
		//群信息写入数据库
		chatdb->my_database_add_new_group(val["user_id"].asString(),val["group_name"].asString());
		
		Json::Value vl;
		vl["cmd"]="create_group_reply";
		vl["result"]="success";
		vl["group_name"]=val["group_name"];
		string  str=Json::FastWriter().write(vl);
	
		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	                 cout<<"server.cpp on line"<<__LINE__<<endl;
        	}
	}else{
		//找到和本次创建的群聊一样了
		
		Json::Value vl;
		vl["cmd"]="create_group_reply";
		vl["result"]="group_exist";
		string  str=Json::FastWriter().write(vl);
		
		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	                 cout<<"server.cpp on line"<<__LINE__<<endl;
        	}
	}
	chatdb->my_database_disconnect();	
}
void Server::server_login(struct bufferevent*bev,Json::Value val){
	chatdb->my_database_connect("chat_mysql2");
	
	//判断用户是否存在
	
	if(!chatdb->my_database_form_info_exist("user_register",val["user_id"].asString())){ //用户不存在
		
		Json::Value val;
		val["cmd"]="login_reply";
		val["result"]="user_not_exist";
		string  str=Json::FastWriter().write(val);
		
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 128 line  bufferevent_write error"<<endl;
		}
		free(data);
		
	}else if(!chatdb->my_dadtabase_chek_user_password(val["user_id"].asString(),val["user_password"].asString())){ //密码不正确
		
		Json::Value val;
		val["cmd"]="login_reply";
		val["result"]="user_password_error";
		string  str=Json::FastWriter().write(val);
	
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 137 line  bufferevent_write error"<<endl;
		}
		free(data);
		cout<<"密码不正确不成啊去111111"<<endl;

	}else{

		//登录成功 初始化好友列表和群信息	

		//把用户添加到server维护的一个在线用户map 在chatlist里面
		chatlist->online_user[val["user_id"].asString()]=bev;	
		
		//chatlist->online_user.push_back();
		//验证成功  处理....
		string frineds_list=chatdb->my_databases_get_friends("friend_relationship",val["user_id"].asString());//返回的是一个好友字符串用|分割
//		cout<<"酷酷酷酷酷"<<endl;
		//cout<<"！！！"<<endl;		
		vector<string> friensv=chatdb->Parse_string(frineds_list);
		Json::Value vals;
		vals["cmd"]="login_reply";
		vals["result"]="success";
		for(int i=0;i<friensv.size();i++){
			if(friensv[i]=="") break;
			vals["friend"].append(friensv[i]);
		}
		//string  str=Json::FastWriter().write(vals);
		//cout<<str<<endl;
//		if(bufferevent_write(bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端
//			cout<<"server.cpp 154 line  bufferevent_write error"<<endl;
//		}
		
		cout<<"目前为止都正常"<<endl;
		//初始化群信息json格式
		chatlist->chatdatabase->my_database_get_group_info(chatlist->group_info,val["user_id"].asString());
		
		//cout<<"这里能运行的同嘛？？"<<endl;
		//类转json
		Json::Value js,values;
		vals["group"]["group_num"]=chatlist->group_info->group_num;
		for(int i=0;i<chatlist->group_info->group_num;i++){
			vals["group"]["group_name"].append(chatlist->group_info->group_name[i]);	
			vals["group"]["group_owner"].append(chatlist->group_info->group_ower[i]);
			vals["group"]["group_member_num"].append(chatlist->group_info->group_member_num[i]);	
			for(int j=0;j<chatlist->group_info->group_member_num[i];j++){
				values["member"].append(chatlist->group_info->group_member[i][j]);
			}
			vals["group"]["group_member"].append(values);
			values["member"].resize(0);
		}
		//返回客户端信息
	//	string info=Json::FastWriter().write(vals);
	//	if(bufferevent_write(bev,info.c_str(),strlen(info.c_str()))<0){//回写客户端
          //               cout<<"server.cpp" <<__LINE__ <<" line  bufferevent_write error"<<endl;
            //     }
		//清空静态变量chatlist->group_info的成员
		chatlist->group_info->group_clear();

		//初始化apply_list信息表,返回给客户端
				
	Apply_list *apply_info=new Apply_list();
	Apply_group_list *apply_group_info=new Apply_group_list();
	chatdb->my_database_get_apply_list(apply_info,apply_group_info,val["user_id"].asString());	
	
	for(int i=0;i<apply_info->user_receive_id.size();i++){
		vals["apply_friend_list"]["user_receive_id"].append(apply_info->user_receive_id[i]);
		vals["apply_friend_list"]["user_send_id"].append(apply_info->user_send_id[i]);
		vals["apply_friend_list"]["flg"].append(apply_info->flg[i]);
	}		
	//初始化apply_group_list,返回客户端	
	for(int i=0;i<apply_group_info->user_receive_id.size();i++){
		vals["apply_group_list"]["user_receive_id"].append(apply_group_info->user_receive_id[i]);
		vals["apply_group_list"]["user_send_id"].append(apply_group_info->user_send_id[i]);
		vals["apply_group_list"]["flg"].append(apply_group_info->flg[i]);
		vals["apply_group_list"]["group_name"].append(apply_group_info->group_name[i]);
	}
		
	delete apply_info;	
	delete apply_group_info;
	//登录成功返回验证消息的状态
	//1.得到user_id的消息验证码
	string verification=chatdb->my_databasse_get_chek_message(val["user_id"].asString());
	//2.写入json	
	vals["message_list_info"]["check_message_flg"]=verification;
	
	//防止粘包包头封装
	string  str=Json::FastWriter().write(vals);
	cout<<"查看封装的数据"<<str<<endl;	
	int len=strlen(str.c_str());
	cout<<"大小："<<len<<endl;	
	char *data=(char*)malloc(len+4);
	int biglen=htonl(len);
	cout<<"转换后的字节序"<<biglen;
	memcpy(data,&biglen,4);
	memcpy(data+4, str.c_str(), len);
	
	if(bufferevent_write(bev,data,len+4)<0){//回写客户端
			cout<<"server.cpp on line"<<__LINE__<<endl;
	}
	
	free(data);
	

		//向在线好友发送在线的信息
		
	string frineds_list2=chatdb->my_databases_get_friends("friend_relationship",val["user_id"].asString());
	vector<string> friensv2=chatdb->Parse_string(frineds_list2);
	Json::Value v2,v3;
	v2["cmd"]="onlineing";
	string strr="";
	for(int i=0;i<friensv2.size();i++){
		if(chatlist->online_user.count(friensv2[i])){
			struct  bufferevent*bv=chatlist->online_user[friensv2[i]];
			v2["user_id"]=val["user_id"];	
		
			v3["cmd"]="friend_online";
			v3["friend_id"]=friensv2[i];

			strr=Json::FastWriter().write(v3);
		//封包	
			int len=strlen(strr.c_str());
			cout<<"封包数据:"<<len<<endl;
			char *data1=(char*)malloc(len+4);
			int biglen=htonl(len);
			memcpy(data1,&biglen,4);
			memcpy(data1+4, strr.c_str(), len);
	
			if(bufferevent_write(bev,data1,len+4)<0){//回写客户端
				cout<<"server.cpp on line"<<__LINE__<<endl;
			}
			
			free(data1);

			strr=Json::FastWriter().write(v2);
			
			len=strlen(strr.c_str());
			char *data2=(char*)malloc(len+4);
			biglen=htonl(len);
			memcpy(data2,&biglen,4);
			memcpy(data2+4, strr.c_str(), len);
	
			if(bufferevent_write(bv,data2,len+4)<0){//回写给好友客户端
	        		cout<<"server.cpp on line"<<__LINE__<<endl;
       			}
			
			free(data2);	
		}
	}
	//登录成功后查看哪个好友是在线的
/*		int start=0,end=0;
		while(1){
			end=frineds_list.find('|',start);
			if(end==-1){
				
				string name=
				//最后一个好友	
				Json::Value value;
				value["cmd"]="friend_login";
				value["friend"]=val["user_id"];
				string  str=Json::FastWriter().write(value);
				if(bufferevent_write(bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端	
					cout<<"server.cpp 167 line  bufferevent_write error"<<endl;
				}
				break;
			}
			string name=frineds_list.substr(start,end-start);
			if(chatlist->online_user.count(name)){ //为真在在线用户中找到了好友发送在线		
				Json::Value value;
				value["cmd"]="friend_login";
				value["friend"]=val["user_id"];
				string  str=Json::FastWriter().write(value);
				if(bufferevent_write(bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端
					cout<<"server.cpp 178 line  bufferevent_write error"<<endl;
				}
			}
			start=end+1;	
		}*/
	}
	
	chatdb->my_database_disconnect();
}
void Server::server_deal_friend_apply(struct bufferevent* bev,Json::Value val){
	
	chatdb->my_database_connect("chat_mysql2");
	int flg=0;
	string send_id="|";
	send_id+=val["send_user_id"].asString();
	string receive_id="|";
	receive_id+=val["receive_user_id"].asString();
	if(val["result"]=="yes"){
		flg=1;
		//修改apply_list
		chatlist->chatdatabase->my_database_update_apply_list_info(val["send_user_id"].asString(),val["receive_user_id"].asString(),"1");
		//修改双方的好友数据库	
		chatlist->chatdatabase->my_database_append_text("friend_relationship","user_friend",send_id,val["receive_user_id"].asString());
		chatlist->chatdatabase->my_database_append_text("friend_relationship","user_friend",receive_id,val["send_user_id"].asString());
	}else{
		chatlist->chatdatabase->my_database_update_apply_list_info(val["send_user_id"].asString(),val["receive_user_id"].asString(),"0");
	}



	
	//判断对方是否在线
	if(chatlist->online_user.count(val["send_user_id"].asString())){
		struct bufferevent*bv=chatlist->online_user[val["send_user_id"].asString()];
		Json::Value vl;
		vl["cmd"]="deal_add_friend_info";
		if(flg){
			vl["result"]="yes";
			string frineds_list=chatdb->my_databases_get_friends("friend_relationship",val["send_user_id"].asString());//返回的是一个好友字符串用|分割
			vector<string> friensv=chatdb->Parse_string(frineds_list);
			for(int i=0;i<friensv.size();i++){
				vl["friend"].append(friensv[i]);
			}
		

		}else{
			vl["result"]="no";
		}
		string  str=Json::FastWriter().write(vl);
		
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bv,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	}
		
		Json::Value v;
		v["cmd"]="deal_add_friend_info";
		if(flg){
			v["result"]="yes";	
			string frineds_list=chatdb->my_databases_get_friends("friend_relationship",val["receive_user_id"].asString());//返回的是一个好友字符串用|分割
			vector<string> friensv=chatdb->Parse_string(frineds_list);
			for(int i=0;i<friensv.size();i++){
				v["friend"].append(friensv[i]);
			}
		}else{
			v["result"]="no";
		}
		string  str=Json::FastWriter().write(v);
		
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	//没处理 2022、8、11 都在线，双方客户端都有好友刷新界面，返回什么呢？？ 返回这个==?  如果是yes同时返回各自的好友列表？？ 2022/9/2 yes 待验证
	//发送在线好友

	chatdb->my_database_disconnect();
}
void Server::server_add_group(struct bufferevent* bev,Json::Value val){
	
	Json::Value vl;
	//判断群是否存在
	if(!chatlist->chatdatabase->my_database_check_exist("group_info","*","group_name",val["group_name"].asString())){
		vl["cmd"]="add_group_reply";
		vl["result"]="group_not_exist";	
		
		string  str=Json::FastWriter().write(vl);
	
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	}else if(chatlist->chatdatabase->my_database_is_already_in_group(val["user_id"].asString(),val["group_name"].asString())){
			//判断用户是否已经在群里	
		vl["cmd"]="add_group_reply";
		vl["result"]="user_in_group";	
		string  str=Json::FastWriter().write(vl);
		
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	
	}else{
		string tmp="";//用来获取群的拥有者的id、
		//向群主发送申请信息，查看群主申请信息表的是否已经有数据，没有相同的数据则把数据插入apply_group_list表中，
		if(!chatlist->chatdatabase->my_database_is_already_group_apply(val["user_id"].asString(),val["group_name"].asString(),tmp)){
			//在apply_group_list中添加双的信息
			chatlist->chatdatabase->my_database_fix_apply_group_list(tmp,val["user_id"].asString(),val["group_name"].asString());	
	
			vl["cmd"]="add_group_reply";
			vl["result"]="already_send";
			string  str=Json::FastWriter().write(vl);
			
			int len=0;	
			char* data=get_data_packet(str,len);
			if(bufferevent_write(bev,data,len)<0){//回写客户端
	        		cout<<"server.cpp on line"<<__LINE__<<endl;
       			}
		}
	//线判断是否在线
	if(chatlist->online_user.count(tmp)){
		chatlist->chatdatabase->my_database_fix_message_list_form(tmp);
		struct bufferevent*bv=chatlist->online_user[tmp];
		Json::Value v;
		v["cmd"]="apply_add_group";
		string  str=Json::FastWriter().write(v);
		
			int len=0;	
			char* data=get_data_packet(str,len);
		if(bufferevent_write(bv,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}

	}
   }	
	//如果群主在线  在向群主发送信息，让其刷新apply_group_list no
	//不在线，用户每次登录成功的时候，会查询一遍apply_group_list yes
	//群主如果同意，判断对方是否在线，如果在线发送yes，如果不在线，则修改apply_group_list的的状态和双方以及群中好的信息 yes

}
void Server::server_group_appply_deal(struct bufferevent* bev,Json::Value val){
	
	chatdb->my_database_connect("chat_mysql2");
	int flg=0;
	if(val["result"]=="yes"){//群主同意
		//修改apply_group_list;
		chatlist->chatdatabase->my_database_update_apply_group_list(val["send_user_id"].asString(),val["receive_user_id"].asString(),"1",val["group_name"].asString());
		//在群里添加群成员 同时修改group_user表，用来表示某个用户所拥有的群
		chatlist->chatdatabase->my_dadtabase_append_group_number_text(val["group_name"].asString(),val["send_user_id"].asString());			
		flg=1;
		chatlist->chatdatabase->my_database_fix_group_user(val["group_name"].asString(),val["send_user_id"].asString());
	}else{
		chatlist->chatdatabase->my_database_update_apply_group_list(val["send_user_id"].asString(),val["receive_user_id"].asString(),"0",val["group_name"].asString());
	}
		//已经同意处理完毕，如果对方在线通知对方	
		if(chatlist->online_user.count(val["send_user_id"].asString())){
			struct bufferevent*bv=chatlist->online_user[val["send_user_id"].asString()];
			Json::Value vl;
			vl["cmd"]="deal_add_group_info";
			if(flg){
				vl["result"]="yes";
				vl["group_name"]=val["group_name"];
			}else{
				vl["result"]="no";
			}
			string  str=Json::FastWriter().write(vl);	
			int len=0;	
			char* data=get_data_packet(str,len);
			if(bufferevent_write(bv,data,len)<0){//回写客户端
	        		cout<<"server.cpp on line"<<__LINE__<<endl;
       			}
		}
	
	chatdb->my_database_disconnect();
}
void Server::server_private_chat(struct bufferevent* bev,Json::Value val){//私聊就是服务器转发消息的一个过程
	//1.不管在不在线，都要先存储信息在mysql中
	chatlist->chatdatabase->my_database_insert_private_recod(val["info"]);		
	//2.如果对方在线,发送数据
	if(chatlist->online_user.count(val["info"]["receivee"].asString())){//对方在线马上发消息
		struct bufferevent* bv=chatlist->online_user[val["info"]["receivee"].asString()];
		
						
		Json::Value vl=val["info"];
		vl["cmd"]="private_chat_reply";
		string  str=Json::FastWriter().write(vl);
			
		cout<<str<<endl;		

		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bv,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	}
}
void Server::server_group_chat(struct bufferevent* bev,Json::Value val){
	
	cout<<"群聊群聊群聊......"<<endl;

	vector<string> number=chatlist->chatdatabase->my_database_get_group_number(val["group_name"].asString());
	for(int i=0;i<number.size();i++){
		if(chatlist->online_user.count(number[i])){
			//在线给其发消息
			struct bufferevent* bv=chatlist->online_user[number[i]];
			string str=Json::FastWriter().write(val);
	
			int len=0;
			char *data=get_data_packet(str,len);
			if(bufferevent_write(bv,data,len)<0){//回写客户端
	        		cout<<"server.cpp on line"<<__LINE__<<endl;
       			}
		}else{
			//不在线，可以存储起来，等用户上限的时候，云端加载一下
	
		}
	}
}
void Server::server_get_group_member(struct bufferevent* bev,Json::Value val){
			
		vector<string> member=chatlist->chatdatabase->my_database_get_group_number(val["group_name"].asString());
		Json::Value vl;
		vl["cmd"]="get_group_member_reply";
		for(int i=0;i<member.size();i++){
			vl["member"].append(member[i]);
		}
		string  str=Json::FastWriter().write(vl);
		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
}
void Server::server_user_offline(struct bufferevent* bev,Json::Value val){
	//从在线用户map里删除
	chatlist->online_user.erase(val["user_id"].asString());
	string frineds_list=chatdb->my_databases_get_friends("friend_relationship",val["user_id"].asString());
	vector<string> friensv=chatdb->Parse_string(frineds_list);
	Json::Value v;
	v["cmd"]="friend_offline";
	for(int i=0;i<friensv.size();i++){
		if(chatlist->online_user.count(friensv[i])){
			struct  bufferevent*bv=chatlist->online_user[friensv[i]];
			v["user_id"]=friensv[i];
			string  str=Json::FastWriter().write(v);
			
		int len=0;
		char *data=get_data_packet(str,len);
		
			if(bufferevent_write(bv,data,len)<0){//回写客户端
	        		cout<<"server.cpp on line"<<__LINE__<<endl;
       			}
		}
	}	
}
void Server::server_send_file(struct bufferevent* bev,Json::Value val){
	Json::Value v;
	if(chatlist->online_user.count(val["to_user_id"].asString())){
		struct bufferevent *to_bev=chatlist->online_user[val["to_user_id"].asString()];
		//启动新线程，创建文件服务器
		int port=9099;//应该随机生成应该端口，并判断端口号在系统中是否被占用，没有被占用才能用 未修改，2022、8、16
		int from_fd=0,to_fd=0;
		thread send_file_thread(send_file_handler,val["length"].asInt(),port,&from_fd,&to_fd);
		send_file_thread.detach();
		v["cmd"]="sned_file_port_reply";
		v["port"]=port;
		v["file_name"]=val["file_name"];
		v["length"]=val["length"];
		string  str=Json::FastWriter().write(v);
		//数据不会被立马发送，会被放到buf的一个缓冲区里面,等满了才会发送
	//	if(bufferevent_write(bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端 port  客户端通过这个端口连接服务器
	        //	cout<<"server.cpp on line"<<__LINE__<<endl;
	//	}
		int len=0;
		char *data=get_data_packet(str,len);
		if(send(bev->ev_read.ev_fd,data,len,0)<0){
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
		}		
		
		cout<<"from_fdssssssssssssssssssssssssssssssssssssssssssssssssssssss"<<from_fd<<endl;	

		int count=0;
		//查看是否连接超时,超时则结束
		while(from_fd<=0){
			count++;
			usleep(100000);//
			if(count==100){//100个100毫秒，10秒
				cout<<"连接超时！！！！！！！"<<endl;
				//取消线程
				//linux的函数 参数为线程号
				pthread_cancel(send_file_thread.native_handle());
				v.clear();
				v["cmd"]="send_file_reply";
				v["result"]="timeout";
				
				string  str=Json::FastWriter().write(v);
				
				 len=0;
				 data=get_data_packet(str,len);
				
				if(bufferevent_write(bev,data,len)<0){//回写客户端 port  客户端通过这个端口连接服务器
	        			cout<<"server.cpp on line"<<__LINE__<<endl;
				}
				return;
			}
		}
		//返回端口号给就接受客户端
		v.clear();	
		v["cmd"]="recv_file_port_reply";
		v["port"]=port;	
		v["file_name"]=val["file_name"];
		v["length"]=val["length"];
		str=Json::FastWriter().write(v);
//		if(bufferevent_write(to_bev,str.c_str(),strlen(str.c_str()))<0){//回写客户端 port  客户端通过这个端口连接服务器
//	        	cout<<"server.cpp on line"<<__LINE__<<endl;
//		}
		
		int len2=0;
		char *data2=get_data_packet(str,len2);
		if(send(to_bev->ev_read.ev_fd,data2,len2,0)<0){
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
		}
	

		count=0;
		//查看是否连接超时,超时则结束
		while(to_fd<=0){
			count++;
			usleep(100000);//
			if(count==100){//100个100毫秒，10秒
				//取消线程
				//linux的函数 参数为线程号
				pthread_cancel(send_file_thread.native_handle());
				v.clear();
				v["cmd"]="send_file_reply";
				v["result"]="timeout";
				
				string  str=Json::FastWriter().write(v);
				
				int lenn=0;
				char *dataa=get_data_packet(str,lenn);
				
				if(bufferevent_write(bev,dataa,lenn)<0){//回写客户端 port  客户端通过这个端口连接服务器
	        			cout<<"server.cpp on line"<<__LINE__<<endl;
				}
			}
		}
		
	}else{
		v["cmd"]="sned_file_reply";
		v["result"]="offline";	
		string  str=Json::FastWriter().write(v);
	
		int len=0;
		char *data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
	        	cout<<"server.cpp on line"<<__LINE__<<endl;
       		}
	}
}
void Server::send_file_handler(int length,int port,int *f_fd,int *t_fd){
	
	cout<<"进入子线程开始！！！！！！！！！！！"<<endl;
	cout<<"length"<<length<<"port:"<<port<<endl;
	int sockfd=socket(AF_INET,SOCK_STREAM,0);	
	if(-1==sockfd){
		return;
	}
	//允许端口复用
	int opt=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));	


	//接收和发送缓冲区设置为1M
	int nRecvBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	int nSendBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	cout<<"bind:"<<bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(sockfd, 50);	
	int len = sizeof(client_addr);
	
	cout<<"到这里还能运行"<<endl;
	*f_fd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
	cout<<"这里嘻嘻嘻嘻嘻嘻嘻嘻"<<endl;
	cout<<"*f_fd="<<*f_fd<<endl;
	*t_fd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
	cout<<"1111111111111111111111111111111111111111111"<<*f_fd<<*t_fd<<endl;
	char buf[MAXSIZE] = {0};
	size_t size, sum = 0;

		while (1)
	{
		size = recv(*f_fd, buf, MAXSIZE, 0);
		if (size <= 0 || size > MAXSIZE)
		{
			break;
		}
		sum += size;
		send(*t_fd, buf, size, 0);
		if (sum >= length)
		{
			break;
		}
		memset(buf, 0, MAXSIZE);
	}

	close(*f_fd);
	close(*t_fd);
	close(sockfd);



/*
	struct sockaddr_in server_addr,client_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=inet_addr(IP);	
	bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	listen(sockfd,10);
	int len=sizeof(client_addr);
	cout<<"开始接收请求"<<endl;
	//接受发送客户端的连接请求
	*f_fd=accept(sockfd,(struct sockaddr*)&client_addr,(socklen_t *)&len);
	//接受接收客户端的连接请求
	*t_fd=accept(sockfd,(struct sockaddr*)&client_addr,(socklen_t *)&len);
	//buffervent 有4k的字节限制 所以用
	char buf[4096]={0};
	size_t size,sum=0;
	while(1){
		size=recv(*f_fd,buf,4096,0);
		sum+=size;
		send(*t_fd,buf,size,0);
		if(sum>=length){
			break;
		}
		memset(buf,0,4096);
	}
	close(*f_fd);
	close(*t_fd);
	close(sockfd);
*/
}
void Server::server_get_apply_info(struct bufferevent* bev,Json::Value val){

	chatdb->my_database_connect("chat_mysql2");
	Json::Value v;
	Apply_list *apply_info=new Apply_list();
	Apply_group_list *apply_group_info=new Apply_group_list();

	chatdb->my_database_get_apply_list(apply_info,apply_group_info,val["user_id"].asString());	
	v["cmd"]="apply_friend_and_group_list";	
	for(int i=0;i<apply_info->user_receive_id.size();i++){
		v["apply_friend_list"]["user_receive_id"].append(apply_info->user_receive_id[i]);
		v["apply_friend_list"]["user_send_id"].append(apply_info->user_send_id[i]);
		v["apply_friend_list"]["flg"].append(apply_info->flg[i]);
	}	
	
	for(int i=0;i<apply_group_info->user_receive_id.size();i++){
		v["apply_group_list"]["user_receive_id"].append(apply_group_info->user_receive_id[i]);
		v["apply_group_list"]["user_send_id"].append(apply_group_info->user_send_id[i]);
		v["apply_group_list"]["flg"].append(apply_group_info->flg[i]);
		v["apply_group_list"]["group_name"].append(apply_group_info->group_name[i]);
	}
			
	delete apply_info;
	delete apply_group_info;
		string  str=Json::FastWriter().write(v);
		int len=0;	
		char* data=get_data_packet(str,len);
		if(bufferevent_write(bev,data,len)<0){//回写客户端
			cout<<"server.cpp 814 line  bufferevent_write error"<<endl;
		}
		free(data);
	chatdb->my_database_set_message_already(val["user_id"].asString());
	chatdb->my_database_disconnect();
}
char* Server::get_data_packet(string str,int &len){
	len=strlen(str.c_str());
	char *data=(char*)malloc(len+4);
	int biglen=htonl(len);
	memcpy(data,&biglen,4);
	memcpy(data+4, str.c_str(), len);
	len+=4;
	return data;
}
Server::~Server(){
	event_base_free(base);
}
