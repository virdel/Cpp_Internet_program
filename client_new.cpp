//封装socket通讯的客户端
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
class ctcpclient{
    public:
    int m_clientfd;
    string  m_ip;
    unsigned short  m_port;
    ctcpclient():m_clientfd(-1){

    };

    bool connect(string& in_ip,const unsigned int short in_port ){
        if(m_clientfd!=-1){
            return false;
        }


        m_clientfd=socket(AF_INET,SOCK_STREAM,0);
        if(m_clientfd==-1){
            perror("socket");
            return false;
        }
        struct hostent* h;
    //用域名或主机名获取ip地址
        if((h=gethostbyname(in_ip.c_str()))==0){
            cout<<"getHostbyname failed.\n"<<endl;
            close();
            return false;

        }
        struct sockaddr_in serveraddr; //存放服务端ip和端口的结构体
        memset(&serveraddr,0,sizeof(serveraddr));
        serveraddr.sin_family=AF_INET;
        memcpy(&serveraddr.sin_addr,h->h_addr,h->h_length);
        serveraddr.sin_port=htons(in_port);
        if(::connect(m_clientfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))!=0){
            perror("connect");
            close();
            return false;
        }
        return true;


    }
    bool send(const string &buffer){
        int iret;
        if((iret=::send(m_clientfd,buffer.c_str(),buffer.size(),0))<=0){
            return false;

        }
        cout<<"发送："<<buffer<<endl;
        return true;
    }

    bool recv(string &buffer,const size_t maxlen){
        buffer.clear();
        buffer.resize(maxlen);
        int readn=::recv(m_clientfd,&buffer[0],buffer.size(),0);
        if(readn<=0){
            return false;
        }
        return true;
    }

    bool close(){
        if(m_clientfd==-1) return false;
        ::close(m_clientfd);
        m_clientfd=-1;
        return true;
    }

    ~ctcpclient(){
        close();

    }



};

int main(int argc,char * argv[]){
    if(argc!=3){
        cout<<"需要输入服务端的ip地址和端口："<<endl;
        return -1;
    };

    ctcpclient client;
    string ip=argv[1];
    client.connect(ip,(unsigned short int)atoi(argv[2]));

    string buffer;
    for(int i=0;i<3;i++){
        int iret;
        buffer="这是第"+to_string(i+1)+"条消息，编号"+to_string(i+1);
  
        if(client.send(buffer)==false){
            perror("send");
        };

        if(client.recv(buffer,1024)==false){
            cout<<"iret:"<<iret<<endl;
            break;
        }
        cout<<"接收："<<buffer<<endl;

        


    }


}