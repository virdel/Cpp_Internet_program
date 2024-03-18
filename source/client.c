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
    if(argc!=3){
        cout<<"需要输入服务端的ip地址和端口："<<endl;
        return -1;
    }
    //第一步：创建客户端的socket.
    int sockid=socket(AF_INET,SOCK_STREAM,0);
    if(sockid==-1){
        perror("socket");
        return -1;
    }
    //向服务器发起连接请求
    struct hostent* h;
    //用域名或主机名获取ip地址
    if((h=gethostbyname(argv[1]))==0){
        cout<<"getHostbyname failed.\n"<<endl;
        close(sockid);
        return -1;

    }
    struct sockaddr_in serveraddr; //存放服务端ip和端口的结构体
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    memcpy(&serveraddr.sin_addr,h->h_addr,h->h_length);
    serveraddr.sin_port=htons(atoi(argv[2]));
    if(connect(sockid,(struct sockaddr*)&serveraddr,sizeof(serveraddr))!=0){
        perror("connect");
        close(sockid);
        return -1;
    }

    //第三步：与服务端通讯，客户端发送一个请求报文后等待客户端回复
    char buffer[1024];
    for(int i=0;i<3;i++){
        int iret;
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"这是第%d条消息，编号%d",i+1,i+1);

        if((iret=send(sockid,buffer,strlen(buffer),0))<=0){
            perror("send");
            break;
        }
        cout<<"发送："<<buffer<<endl;
        memset(buffer,0,sizeof(buffer));
        if((iret=recv(sockid,buffer,sizeof(buffer),0))<=0){
            cout<<"iret:"<<iret<<endl;
            break;
        }
        cout<<"接收："<<buffer<<endl;

        


    }

    //第四步：关闭
    close(sockid);



}