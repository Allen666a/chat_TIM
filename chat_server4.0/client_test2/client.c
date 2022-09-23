#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <json-c/json.h>
#define SERV_PORT 9000
#define SERV_IP "10.0.24.15"
int main(void){
    int cfd;
    struct sockaddr_in serv_addr;
//    char buf[BUFSIZ];
   // socklen_t serv_addr_len;
//    int n;
    cfd=socket(AF_INET,SOCK_STREAM,0);
    //不绑定ip linux会隐式绑定
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,SERV_IP,&serv_addr.sin_addr.s_addr); 
    connect(cfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    

   /* while (1)
    {
        fgets(buf,sizeof(buf),stdin);
        write(cfd,buf,strlen(buf));
       // n=read(cfd,buf,sizeof(buf));
     //   write(STDOUT_FILENO,buf,n); //STDOUT_FILENO：向屏幕输出
    }*/
	//测试注册功能    jsonC库
	struct json_object *obj=json_object_new_object(); 
	json_object_object_add(obj,"cmd",json_object_new_string("add_apply_deal"));	
	json_object_object_add(obj,"send_user_id",json_object_new_string("花子"));
//	json_object_object_add(obj,"user_name",json_object_new_string("李信"));
	json_object_object_add(obj,"receive_user_id",json_object_new_string("韩立"));
	json_object_object_add(obj,"result",json_object_new_string("yes"));
	
	const char* buf=json_object_to_json_string(obj);
	send(cfd,buf,strlen(buf),0);
	
	//while(1){	
	char s[2048]={0};
	recv(cfd,s,sizeof(s),0);
	printf("服务器回复：%s",s);
		memset(s,0,sizeof(s));	
	//}
	while(1){}
    close(cfd);
    return 0;
}
