/////////////////////////////////////////////////////////////////////////
// File Name       : webclient.c                                       //
// Date            : 2020/06/04 ~ 2020/06/10                           // 
// OS              : Ubuntu 18.04.4 LTS                                // 
// Student Name    : Seung Hoon Jeong                                  //
// Student ID      : 2015707003                                        //
// ------------------------------------------------------------------- //
// Title : Network Programming Assignment #12-2                        //
// Description : Simple HTTP Web Browser                               //
/////////////////////////////////////////////////////////////////////////

/* 필요한 헤더 파일 선언 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#define BUF_SIZE 1024

////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : int main(int argc, char **argv)                                                     //
// ============================================================================================== //
// Input: gcc -o telnet webclient.c ./[telnet] [ssl.kw.ac.kr] [80]                                //
// Output: 1. success HTTP/1.1 200 OK ...                                                         //
//         2. fail : HTTP/1.1 400 Bad Request ...                                                 //
// ============================================================================================== //      
// Purpose: HTTP Command Test using telnet command                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr;
    
    char first_buff[BUF_SIZE] = {0};
    char second_buff[BUF_SIZE] = {0};
    char third_buff[BUF_SIZE] = {0};
    char buff[BUF_SIZE] = {0};
    char *url = "ssl.kw.ac.kr";
    struct hostent *host;

    /* argument count exception handling */
    if(argc != 3) {
        printf("Usage : %s [URL] [PORT_NUMBER]\n",argv[0]);
        exit(0);
    }

    /* open socket */
    if((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0) {
        perror("sock");
        exit(0);
    }

    /* get hostname */
    if((host = gethostbyname(url)) < 0) {
        perror("gethostbyname");
        exit(0);
    }

    memset((char *)&servaddr,'\0',sizeof(servaddr)); // initialize server socket information struct to zero
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = *(u_long *)host->h_addr_list[0]; // 4 byte address casting
    servaddr.sin_port = htons(atoi(argv[2])); // short data(port number) to network byte order
    
    printf("Trying %s...\n",inet_ntoa(servaddr.sin_addr));
    /* connect socket */
    if(connect(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }
    else printf("connect to %s.\n",host->h_name);
    printf("Escape character is '^]'.\n"); 

    while(1)
    {
        ///////////////////text input///////////////////
        read(STDIN_FILENO, first_buff, BUF_SIZE);
        /* first line exception handling */
        if(strcmp(first_buff,"GET / HTTP/1.1\n")) {
            write(sockfd,first_buff,sizeof(first_buff)); // 입력 틀리면 bad request 응답받기 위함
            break;
        }
        else {
            strcat(buff,first_buff);
            bzero(first_buff,sizeof(first_buff));
        }
        read(STDIN_FILENO, second_buff, BUF_SIZE);
        /* second line exception handling */
        if(strcmp(second_buff,"Host: ssl.kw.ac.kr\n")) {
            write(sockfd,second_buff,sizeof(second_buff));
            
        }
        else {
            strcat(buff,second_buff);
            bzero(second_buff,sizeof(second_buff));
            
        }
        read(STDIN_FILENO, third_buff, BUF_SIZE);
        /* third line exception handling */
        if(strcmp(third_buff,"\n")) {
            write(sockfd,third_buff,sizeof(third_buff));
            bzero(third_buff,sizeof(third_buff));
        }
        else {
            strcat(buff,third_buff);
            bzero(third_buff,sizeof(third_buff));
            break;
        }
    }
    
    if(!strcmp(buff,"GET / HTTP/1.1\nHost: ssl.kw.ac.kr\n\n")) {
        /* Sending HTTP Request */
        if(write(sockfd,"GET / HTTP/1.1\r\nHost:ssl.kw.ac.kr\r\n\r\n",sizeof("GET / HTTP/1.1\r\nHost: ssl.kw.ac.kr\r\n\r\n")) < 0) {
            write(STDERR_FILENO, "write error!!\n", sizeof("write error!!\n"));
            exit(1);
        }

        /* Receiving HTTP Reply (OK Request) */
        if(read(sockfd,buff,BUF_SIZE-1) < 0) {
            write(STDERR_FILENO, "read error!!\n", sizeof("read error!!\n"));
            exit(0);
        }
        else printf("%s",buff);
        sleep(4);
    }
    
    /* Receiving HTTP Reply (Bad Request) */
    if(read(sockfd,buff,BUF_SIZE-1) < 0) {
        write(STDERR_FILENO, "read error!!\n", sizeof("read error!!\n"));
        exit(0);
    }
    else printf("%s",buff); 
    
    /* close socket */ 
    close(sockfd);
    printf("Connection closed by foreign host.\n");
    return 0;
}
