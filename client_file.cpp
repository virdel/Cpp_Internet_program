//封装socket通讯的客户端
#include<iostream>
#include<fstream>
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
        return true;
    }

    //发送二进制数据
    bool send(void *buffer, const size_t size){
        int iret;
        if((iret=::send(m_clientfd,buffer,size,0))<=0){
            return false;

        }
        return true;
    }

    bool sendfile(const string& filename,const size_t filesize)
    {
        ifstream fin(filename,ios::binary);
        if(fin.is_open()==false){
            cout<<"打开文件"<<filename<<"失败"<<endl;
        }
        int onread=0;
        int totalbytes=0;
        char buffer[7];

        while(totalbytes<filesize){
            memset(buffer,0,sizeof(buffer));

            if(filesize-totalbytes>7) onread=7;
            else onread=filesize-totalbytes;

            fin.read(buffer,onread);
            if(!send(buffer,onread)) return false;
            totalbytes+=onread;
        }
        return true;



    };
    

    bool recv(string &buffer,const size_t maxlen){
        buffer.clear();
        buffer.resize(maxlen);
        int readn=::recv(m_clientfd,&buffer[0],buffer.size(),0);
        if(readn<=0){
            return false;
        }
        return true;
    }
    bool recv(void* buffer,const size_t size){

        int readn=::recv(m_clientfd,buffer,size,0);
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
    if(argc!=5){
        cout<<"需要输入服务端的ip地址和端口："<<endl;
        return -1;
    };

    ctcpclient client;
    string ip=argv[1];
    client.connect(ip,(unsigned short int)atoi(argv[2]));
    struct st_fileinfo{
        char filename[256];
        int filesize;
    } fileinfo;
    memset(&fileinfo,0,sizeof(fileinfo));
    strcpy(fileinfo.filename,argv[3]);
    fileinfo.filesize=atoi(argv[4]);
    if(client.send(&fileinfo,sizeof(fileinfo))==false){
        perror("send");
        return -1;
    }
    cout<<"发送文件信息的结构体"<<fileinfo.filename<<"("<<fileinfo.filesize<<")."<<endl;
    string buffer;
    //接收确认报文
    if(!client.recv(buffer,1024)){
        return -1;
    }

    cout<<"回应报文："<<buffer<<endl;

    //发送文件
    if(!client.sendfile(fileinfo.filename,fileinfo.filesize)){
        return -1;
    }
    


    return 0;


   

}