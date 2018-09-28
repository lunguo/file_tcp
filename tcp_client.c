#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>


#pragma pack(1)
typedef struct _file_info
{
    char file_name[51];
    unsigned int file_size;
}file_info;
#pragma pack()

int main(int argc, char** argv)
{
    if(argc!=3)
    {
        printf("参数错误!\n");
        exit(1);
    }

    //第一步：创建套接字
    int sock;
    sock=socket(AF_INET,SOCK_STREAM,0);

    //第二步：绑定地址（可以省略）
    struct sockaddr_in myaddr;
    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=INADDR_ANY;
    myaddr.sin_port=htons(9999);
   

    if(bind(sock, (struct sockaddr*)&myaddr, sizeof(myaddr))==-1)
    {
        perror("bind");
    }

    //第三部：连接服务器
    struct sockaddr_in srvaddr;
    srvaddr.sin_family=AF_INET;
    srvaddr.sin_addr.s_addr=inet_addr(argv[1]);
    srvaddr.sin_port=htons(atoi(argv[2]));
    
    printf("正在尝试连接...\n");
    sleep(2);
    while(connect(sock, (struct sockaddr*)& srvaddr, sizeof(srvaddr))!=0)
    {
        perror("connect error");
        printf("正在尝试连接...\n");
        sleep(2);
    }
        printf("连接成功!");

    //第四步：收发数据
    
    file_info fi;
    int ret;

    ret=recv(sock, &fi, sizeof(fi),0);

    if(ret>0)
    {
        int n=3+strlen(fi.file_name)+1;
        char a[n];
        strcpy(a,"vim ");

        strcat(a,fi.file_name);
        system(a);
        FILE* fp=fopen(fi.file_name,"w");

        char msg1[1024];
        int cnt=0;
        
        if(fi.file_size<1024 || fi.file_size==1024)
        {
            ret=recv(sock, msg1, 1024,0);
            if(ret>0)
                fwrite(msg1,fi.file_size-cnt*1024,1,fp);
        }
        else
        {
            while(1)
            {

                ret=recv(sock, msg1, 1024,0);

                if(ret>0)
                {
                    ++cnt;
                    if(cnt<(fi.file_size/1024+1))
                        fwrite(msg1,sizeof(msg1),1,fp);

                    else break;

                }

            }
            fwrite(msg1,fi.file_size-cnt*1024,1,fp);

        }

    fclose(fp);
    }
    //断开连接（关闭套接字）
    close(sock);

    return 0;
}
