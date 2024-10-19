#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<sys/time.h>
#define PORT_COUNT 20
#define PORT 2000
#define ADDR "192.168.208.128"
#define CONNECTIONS 1000000
#define EVENT_COUNT 1024
#define BUF_SIZE 1024

int connections;
struct timeval begin,cur;
typedef void (*func)(int fd);
typedef int (*msg_handler)(char* msg,int length,char* response);
msg_handler kvs_handler;
struct conn
{
    int fd;
    func recvf_cb;
    func send_cb;
    char rbuf[BUF_SIZE];
    char wbuf[BUF_SIZE];
};
struct conn conn_list[CONNECTIONS];
int kvs_request(struct conn* c){
    char resp[1024]={0};
    kvs_handler(c->rbuf,strlen(c->rbuf),resp);
    memcpy(c->wbuf,resp,strlen(resp));
}
int kvs_response(struct conn* c){

}

int epfd;
void accept_callback(int fd);
void recv_callback(int fd);
void send_callback(int fd);
void registe(int fd);
void recv_callback(int fd)
{
    int ret=recv(fd,conn_list[fd].rbuf,BUF_SIZE,0);
    if(ret==-1)
    {
        perror("failed to recv");
        exit(1);
    }
    else if(ret==0)
    {
        printf("fd:%d had disconnect!\n",fd);
        conn_list[fd].fd=0;
        memset(conn_list[fd].rbuf,'\0',BUF_SIZE);
        memset(conn_list[fd].wbuf,'\0',BUF_SIZE);
        conn_list[fd].recvf_cb=NULL;
        conn_list[fd].send_cb=NULL;
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        close(fd);
        return;
    }
    //printf("recv:%s\n",conn_list[fd].rbuf);

    kvs_request(&conn_list[fd]);

    memset(conn_list[fd].rbuf,'\0',BUF_SIZE);
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=EPOLLOUT;
    epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
    return;
}
void accept_callback(int fd)
{
    struct sockaddr_in cliaddr;
    int len=sizeof(cliaddr);
    int acfd=accept(fd,(struct sockaddr*)&cliaddr,&len);
    if(acfd==-1)
    {
        perror("failed to accept");
        exit(1);
    }
    connections++;
    if(connections%1000==0)
    {
        gettimeofday(&cur,NULL);
        int val=(cur.tv_sec-begin.tv_sec)*1000+(cur.tv_usec-begin.tv_usec)/1000;
        memcpy(&begin,&cur,sizeof(begin));
        printf("connections:%d  time:%d\n",connections,val);
    }
    registe(acfd);
    return;
}
void send_callback(int fd)
{
    //strcpy(conn_list[fd].wbuf,"response\n");
    if(send(fd,conn_list[fd].wbuf,strlen(conn_list[fd].wbuf),0)==-1)
    {
        perror("failed to send");
        exit(1);
    }
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
    memset(conn_list[fd].wbuf,'\0',BUF_SIZE);
    return;
}
void registe(int fd)
{
    conn_list[fd].fd=fd;
    conn_list[fd].recvf_cb=recv_callback;
    conn_list[fd].send_cb=send_callback;
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=EPOLLOUT;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
}
void init_server(int port)
{
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("failed to create socket");
        exit(1);
    }
    int opt=1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr=inet_addr(ADDR);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    if(bind(lfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in))==-1)
    {
        perror("failed to bind");
        exit(1);
    }
    if(listen(lfd,10)==-1)
    {
        perror("failed to listen!");
        exit(1);
    }
    conn_list[lfd].fd=lfd;
    conn_list[lfd].recvf_cb=accept_callback;
    conn_list[lfd].send_cb=send_callback;
    struct epoll_event ev;
    ev.data.fd=lfd;
    ev.events=EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
    
}
int reactor_start(msg_handler handler)
{
    kvs_handler=handler;
    gettimeofday(&begin,NULL);
    epfd=epoll_create(1);
    for(int i=0;i<PORT_COUNT;i++)
    {
        init_server(i+PORT);
    }
    
    struct epoll_event evs[EVENT_COUNT];
    while(1)
    {
        int num=epoll_wait(epfd,evs,EVENT_COUNT,-1);
        for(int i=0;i<num;i++)
        {
            if(evs[i].events&EPOLLIN)
            {
                conn_list[evs[i].data.fd].recvf_cb(evs[i].data.fd);
            }
            else if(evs[i].events&EPOLLOUT)
            {
                conn_list[evs[i].data.fd].send_cb(evs[i].data.fd);
            }
        }
    }
    

    return 0;
}