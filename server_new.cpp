#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<arpa/inet.h>
#include<string>
using namespace std;

class ctcpserver{
    private:
     int m_lisenfd;
     int m_clientfd;
     string m_clientip;
     unsigned short  m_port;
    public:
     ctcpserver():m_lisenfd(-1),m_clientfd(-1){

     };
     bool initializer(const unsigned short in_port){
            //创建服务端socket;
        if(m_lisenfd!=-1) return false;

        m_lisenfd=socket(AF_INET,SOCK_STREAM,0);

        if(m_lisenfd==-1){
            perror("socket");
            return false;
        }
        //绑定端口和用于通信的网卡ip地址
        struct sockaddr_in servaddr;
        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        //将全部IP用于通讯
        servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        //inet("192.168.0.0")转为大端序
        //atoi()函数可以将字符串转为整数
        servaddr.sin_port=htons(in_port);

        //绑定
        if(bind(m_lisenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){
            perror("bind");
            close(m_lisenfd);
            return false;
        }

        //把socket设置为监听状态
        if(listen(m_lisenfd,5)!=0){
            perror("listen");
            close(m_lisenfd);
            return false;
        }
        return true;

     };

     //受理客户端的连接，如果没有连接的客户端，accept函数将阻塞等待；
    bool accept(){
        struct sockaddr_in servaddr;
        socklen_t addrlen=sizeof(servaddr);
        m_clientfd=::accept(m_lisenfd,(struct sockaddr*)&servaddr,&addrlen);
        if(m_clientfd==-1){
            return false;
        }
        m_clientip=inet_ntoa(servaddr.sin_addr);
        return true;
    }
    const string& clientip() const{
        return m_clientip;
    }

    bool send(const string & buffer){
        if(m_clientfd==-1) return false;
        if(::send(m_clientfd,buffer.data(),buffer.size(),0)<=0){
            return false;
        }
        return true;
    }
    bool recv(string& buffer,const size_t maxlen){
        buffer.clear();
        buffer.resize(maxlen);
        int readn=::recv(m_clientfd,&buffer[0],buffer.size(),0);
        if(readn<=0){
            buffer.clear();
            return false;
        }

        buffer.resize(readn);
        return true;
    }

    bool closelisten(){
        if(m_lisenfd==-1) return false;
        ::close(m_clientfd);
        m_clientfd=-1;
        return true;

    }
    bool closeclient(){
        if(m_clientfd==-1) return false;
        ::close(m_clientfd);
        m_clientfd=-1;
        return true;
    }


     ~ctcpserver(){
        closeclient();
        closelisten();

     };

};
//信号处理函数
void FathEXIT(int sig);
void ChldEXIT(int sig);


int main(int argc,char * argv[]){
    if(argc!=2){
        cout<<"需要设置通信端口："<<endl;//服务器必须开启对应端口，eg 5005
        return -1;
    }
    ctcpserver server;

    if(server.initializer(atoi(argv[1]))==false){
        perror("initialization error");
        return -1;
    }
    while (true)
    {
        if(!server.accept()){
            perror("accept error");
            return -1;
        }
        int pid=fork();
        if(pid==-1){
            perror("fork error;");
            return -1;
        }
        if(pid>0){
            server.closeclient();
            continue;//父进程
        }
        server.closelisten();

        //子进程与客户端通信

        cout<<"客户端已连接"<<endl;
        string buffer;
        while(true){
            buffer.clear();

            if(!server.recv(buffer,1024)){
                cout<<"receive failed:"<<endl;
                break;
            }

            cout<<"接收："<<buffer<<endl;
            buffer+="ok";
            if(!server.send(buffer)){
                perror("send error");
                break;
            }
            cout<<"发送："<<buffer<<endl;
        }
        return 0;
    }
    return 0;
}