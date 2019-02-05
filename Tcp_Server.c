#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        perror("argument error! ./Usage port");
        return 1;
    }
    int lst_fd, cli_fd;
    int i, ret;
    socklen_t len;
    int fd_list[1024];   //文件描述符数组
    fd_set read_fds;     //在select下监视的文件描述符位图
    sockaddr_in lst_addr;
    sockaddr_in cli_addr;
    lst_fd = socket(AF_INET,SOCK_STREAM,0);
    if(lst_fd < 0){
        perror("socket error!");
        return 2;
    }
    lst_addr.sin_family = AF_INET;
    lst_addr.sin_port = htons(atoi(argv[1]));
    lst_addr.sin_addr.s_addr = INADDR_ANY;
    len = sizeof(lst_addr);
    ret = bind(lst_fd,(struct sockaddr*)&lst_addr,len);
    if(ret < 0){
        perror("bind error!");
        return 3;
    }
    if(listen(lst_fd,5) < 0){
        perror("listen error!");
        return 4;
    }
    for(i = 0; i < 1024; i++){   //初始化数组
        fd_list[i] = -1;
    }
    fd_list[0] = lst_fd;         //将监听描述符放在集合里边
    while(1){
        int max_fd = lst_fd;
        //清空描述符集合
        FD_ZERO(&read_fds);
        for(i = 0; i < 1024; i++){
            //将存在的描述符添加到集合中
            if(fd_list[i] != -1){
                FD_SET(fd_list[i],&read_fds);
            }
            //找出存在的最大的描述符
            if(fd_list[i] > max_fd){
                max_fd = fd_list[i];
            }
        }
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(max_fd + 1,&read_fds,NULL,NULL,&tv);
        if(ret < 0){
            perror("select error!");
            continue;
        }
        else if(ret == 0){
            printf("timeout!!\n");
            continue;
        }
        for(i = 0;i < 1024; i++){
            if(fd_list[i] < 0) continue;
            //判断哪个描述符就绪了
            if(FD_ISSET(fd_list[i],&read_fds)){
                //如果就绪的描述符是监听描述符,则就可以接收了
                if(fd_list[i] == lst_fd){
                    cli_fd = accept(lst_fd,(struct sockaddr*)&cli_addr,&len);
                    if(cli_fd < 0){
                        perror("accept error!");
                        continue;
                    }
                    for(i = 0; i < 1024; i++){
                        if(fd_list[i] == -1){
                            fd_list[i] = cli_fd;
                            break;
                        }
                    }
                }
                else{
                    //就绪描述符不是监听描述符,代表有客户端连接的数据到了
                    char buf[1024] = {0};
                    ret = recv(fd_list[i],buf,sizeof(buf)-1,0);
                    if(ret <= 0){
                        close(fd_list[i]);
                        fd_list[i] = -1;
                        continue;
                    }
                    printf("%s\n",buf);
                    send(fd_list[i],"What???",7,0);
                }
            }
        }
    }

    return 0;
}
