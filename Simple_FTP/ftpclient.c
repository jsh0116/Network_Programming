/////////////////////////////////////////////////////////////////////////
// File Name       : ftpclient.c                                       //
// Date            : 2020/06/09 ~ 2020/06/15                           // 
// OS              : Ubuntu 18.04.4 LTS                                // 
// Student Name    : Seung Hoon Jeong                                  //
// Student ID      : 2015707003                                        //
// ------------------------------------------------------------------- //
// Title : Network Programming Assignment #13-1                        //
// Description : Simple FTP Client                                     //
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
// Input: gcc -o telnet ftpclient.c,  ./[telnet] [223.194.7.95] [21]                              //
// Output: 331 Please specify the password., 230 Login successful.                                //
//         227 Entering Passive Mode (223,194,7,95,247,30).                                       //
//         150 Opening BINARY mode data connection for welcome.txt (54 bytes).                    //
//         226 Transfer complete.                                                                 //
//                                                                                                // ==============================================================================================    //      
// Purpose: FTP Command Test using telnet command                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr;
    
    char buff[BUF_SIZE] = {0};

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

    memset(&servaddr,'\0',sizeof(servaddr)); // initialize server socket information struct to zero
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2])); // short data(port number) to network byte order
    
    printf("Trying %s...\n",argv[1]); // trying to enter ftp server
    /* connect socket */
    if(connect(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }
    else printf("connected to %s.\n",argv[1]);
    printf("Escape character is '^]'.\n");

    bzero(buff,sizeof(buff));
    n = read(sockfd,buff,BUF_SIZE-1);
    printf("%s",buff); // print 220 (vsFTPd 3.0.3) or welcome.txt file contents

    /* .txt 파일 내용 출력 될 때 */
    if(strcmp(buff,"220 (vsFTPd 3.0.3\n"))
    {
        FILE* fp = fopen("welcome.txt", "w");
        if (fp == NULL)
            perror("file");
        fwrite(buff, 1, n, fp); // write buffer contents
        memset(buff, '\0', BUF_SIZE);
        fclose(fp);
        close(sockfd);
        printf("Connection closed by foreign host.\n");
        return 0;
    }
    bzero(buff,sizeof(buff));
    
    char msg[BUF_SIZE] = "USER elec\r\nPASS comm\r\nPASV\r\nRETR welcome.txt\r\n"; 
    write(sockfd, msg, sizeof(msg)); // send message
    
    while(1)
    {
        read(sockfd, buff, BUF_SIZE-1);
        printf("%s", buff); // print result
        bzero(buff,sizeof(buff));
        
    }

    /* close socket */ 
    close(sockfd);
    printf("Connection closed by foreign host.\n");
    return 0;
}
