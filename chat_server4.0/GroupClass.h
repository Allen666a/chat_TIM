#ifndef CHART_GROUP
#define CHART_GROUP
#include <vector>
#include <unordered_map>
using namespace std;
class GroupClass{

public:
	vector<int> group_member_num;//每个群的成员数量
	int group_num; //群的数量
	vector<string> group_name; //群名字
	vector<string> group_ower;//群的拥有者
	//unordered_map<string,vector<string>> group_member;//群的成员  群成员
	vector<vector<string> > group_member;
	void group_clear();
	GroupClass();
	~GroupClass();
};
#endif
