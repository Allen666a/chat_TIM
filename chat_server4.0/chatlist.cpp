#include "chatlist.h"

ChatInfo::ChatInfo(){

	//目前还没写释放的代码
//	 online_user=new Chat_online_member_class();
	 group_info=new GroupClass();

	//group_info容器中中添加群信息
	//初始化数据库对象
	chatdatabase=new ChatDataBase();
	chatdatabase->my_database_connect("chat_mysql2");
	
	//获取数据库中群组表中的信息  引用方式
//	chatdatabase->my_database_get_group_info(group_info);
	
	//	
	//group_info.push_abck(groupclass);
	//online_user.push_back(onlin_member_class);
	
	
	/*
	for(int i=0;i<group_num;i++){
		Group g;
		g.name=group_name[i];		
		g.next=new list<GroupUser>;
		
		string group_member_name;
		chatdatabase->my_databes_get_group_member(group_name[i],group_member_name);
		
		int start=0,end=0;
		GroupUser u;
	while(true){
			end=group_member_name.find('|',start);
			if(end==-1){
				break;
			}
				u.name=group_member_name.substr(start,end-start);
			g.next->push_back(u);
			start=end+1;
			u.name.clear();
		}
			u.name=group_member_name.substr(start,group_member_name.size()-start);
			g.next->push_back(u);
			group_info->push_back(g);
	}
	
	for(list<Group>::iterator it=group_info->begin();it!=group_info->end();it++){
			cout<<"群名:"<<it->name<<endl;
			for(list<GroupUser>::iterator it2=it->next->begin();it2!=it->next->end();it2++){
				cout<<"群成员为:"<<it2->name<<" ";
			}
			cout<<endl;
		}	
	*/
/*2022/8/4	
	cout<<"群的数量"<<group_info->group_num<<endl;
	for(int i=0;i<group_info->group_num;i++){
		cout<<"群的名字:"<<group_info->group_name[i]<<"  ";
		cout<<"群的拥有者:"<<group_info->group_ower[i]<<"  ";
		cout<<"群成员数量:"<<group_info->group_member_num[i]<<" ";	
	//	for(int j=0;j<group_info->group_member_num[i];j++){
			for(int k=0;k<group_info->group_member_num[i];k++){		
				cout<<"成员:"<<group_info->group_member[group_info->group_ower[i]][k]<<" ";
			}
	//	}
		cout<<endl;
	}
	
*/	
	//cout<<"群初始化成功！！"<<endl;
	//chatdatabase->my_database_disconnect();
}
ChatInfo::~ChatInfo(){

}
