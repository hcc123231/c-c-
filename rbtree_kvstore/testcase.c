#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/time.h>
#define RBUF_SIZE 1024
void sendMessege(int cfd,char* msg,int length){
    if(send(cfd,msg,length,0)==-1){
        perror("sendMessege\n");
        return;
    }
}
void recvMessege(int cfd,char* msg){
    if(recv(cfd,msg,RBUF_SIZE,0)==-1){
        perror("recvMessege\n");
        return;
    }
}
void testCase(int cfd,char* msg,char* pattern,char* casename){
    sendMessege(cfd,msg,strlen(msg));
    char result[RBUF_SIZE]={0};
    recvMessege(cfd,result);
    /*if(strcmp(result,pattern)==0){
        printf("==>> PASS ->%s  \n",casename);
    }else{
        printf("==>> FAILED ->%s  %s!=%s\n",casename,result,pattern);
        printf("error\n");
        exit(1);
    }*/
   if(strcmp(result,pattern)!=0){
    exit(0);
   }
}
int connect_tcpserver(const char* ip,unsigned short port){
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    if(connect(cfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr))!=0){
        printf("connect_tcpserver\n");
        return -1;
    }

    printf("success\n");
    return cfd;
}
void array_testcase(int cfd){
    /*testCase(cfd,"SET TEA 100","ok\r\n","test1");
    testCase(cfd,"GET TEA","100\r\n","test2");
    testCase(cfd,"MOD TEA 200","ok\r\n","test3");
    testCase(cfd,"GET TEA","200\r\n","test4");
    testCase(cfd,"DEL TEA","ok\r\n","test5");
    testCase(cfd,"GET TEA","not exist\r\n","test6");
    testCase(cfd,"MOD TEA","not exist\r\n","test7");
    testCase(cfd,"EXIST TEA","not exist\r\n","test8");
    testCase(cfd,"SET TEA 300","ok\r\n","test9");
    testCase(cfd,"SET TEA 500","exist\r\n","test10");
    testCase(cfd,"DEL TEA","ok\r\n","test11");*/
    
    testCase(cfd, "RSET Teacher King", "ok\r\n", "SET-Teacher");
	testCase(cfd, "RGET Teacher", "King\r\n", "GET-Teacher");
    testCase(cfd, "RMOD Teacher Darren", "ok\r\n", "MOD-Teacher");
	testCase(cfd, "RGET Teacher", "Darren\r\n", "GET-Teacher");
	testCase(cfd, "REXIST Teacher", "exist\r\n", "EXIST-Teacher");
	testCase(cfd, "RDEL Teacher", "ok\r\n", "DEL-Teacher");
	testCase(cfd, "RGET Teacher", "not exist\r\n", "GET-Teacher");
	testCase(cfd, "RMOD Teacher KING", "not exist\r\n", "MOD-Teacher");
	testCase(cfd, "REXIST Teacher", "not exist\r\n", "GET-Teacher");


}
void array_testcase_10w(int cfd){
    int count=10000;
    int i=0;
    for(i=0;i<count;i++){
        array_testcase(cfd);
    }
}
int main(int argc,char* argv[]){
    if(argc<3){
        printf("lack two param\n");
        return -1;
    }
    struct timeval time_begin;
    struct timeval time_end;
    
    int cfd=connect_tcpserver(argv[1],atoi(argv[2]));
    gettimeofday(&time_begin,NULL);
    array_testcase_10w(cfd);
    gettimeofday(&time_end,NULL);

    //array_testcase(cfd);
    int waste=(time_end.tv_sec-time_begin.tv_sec)*1000+(time_end.tv_usec-time_begin.tv_usec)/1000;
    printf("time:%d\n",waste);
    return 0;
}