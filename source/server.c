#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<arpa/inet.h>
using namespace std;
int main(int argc,char * argv[]){
    if(argc!=2){
        cout<<"需要设置通信端口："<<endl;//服务器必须开启对应端口，eg 5005
        return -1;
    }

    //创建服务端socket;
    int listenid=socket(AF_INET,SOCK_STREAM,0);

    if(listenid==-1){
        perror("socket");
        return -1;
    }
    //绑定端口和用于通信的网卡ip地址
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    //将全部IP用于通讯
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    //inet("192.168.0.0")转为大端序
    //atoi()函数可以将字符串转为整数
    servaddr.sin_port=htons(atoi(argv[1]));

    //绑定
    if(bind(listenid,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){
        perror("bind");
        close(listenid);
        return -1;
    }

    //把socket设置为监听状态
    if(listen(listenid,5)!=0){
        perror("listen");
        close(listenid);
        return -1;
    }
    //受理客户端连接请求
    int clientid=accept(listenid,0,0);
    if(clientid==-1){
        perror("accept");
        close(listenid);
        return -1;
    }

    cout<<"客户端已连接"<<endl;

    char buffer[1024];
    while(true){
        int iret;
        memset(&buffer,0,sizeof(buffer));
        if(iret=recv(clientid,buffer,sizeof(buffer),0)<=0){
            cout<<"iret:"<<iret<<endl;
            break;
        }

        cout<<"接收："<<buffer<<endl;
        strcpy(buffer,"ok");
        if((iret=send(clientid,buffer,strlen(buffer),0))<=0){
            perror("send");
            break;
        }
        cout<<"发送："<<buffer<<endl;



    }
    close(clientid);
    close(listenid);


    




}