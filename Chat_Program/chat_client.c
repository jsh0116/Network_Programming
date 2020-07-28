//////////////////////////////////////////////////////////////////////////
// File Name       : chat_client.c                                      //
// Date            : 2020/06/20 ~ 2020/06/30                            // 
// OS              : Ubuntu 18.04.4 LTS                                 // 
// Student Name    : Seung Hoon Jeong                                   //
// Student ID      : 2015707003                                         //
// -------------------------------------------------------------------  //
// Title : Network Programming Term Project                             //
// Description : Linux 환경 멀티 채팅 서버 및 클라이언트 프로그램 개발      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFSIZE 1024
#define NAME 20

char name[NAME]; // each func use this variable

///////////////////////////////////////////////////////////////////////
// Function : void *send_(void *sock_fd)                             //
// ================================================================= //
// Input: x                                                          //
// Output: x                                                         //
// Purpose: send message to server                                   //
///////////////////////////////////////////////////////////////////////
void *send_(void *sock_fd)
{
    int sock = *(int *)sock_fd;
	char named_msg[BUFSIZE+NAME];
	char msg[BUFSIZE];
	while(1)
	{
		/* 메세지 보내기전 메모리 초기화 필수 */
		memset((void *)msg, 0x00, BUFSIZE);
		memset((void *)named_msg, 0x00, BUFSIZE+NAME);
		read(STDIN_FILENO, msg, BUFSIZE); // input message
		/* @exit 입력했을 때 */
		if(!strcmp(msg, "@exit\n")) {
            printf("채팅이 종료되었습니다.\n");
			close(sock);
			exit(0);

		}
		/* @show 입력했을 때 */
        else if(!strcmp(msg,"@show\n")) {
            write(sock,msg,strlen(msg));
        }

		/*그 이외 입력했을 때 */
        else {
            sprintf(named_msg,"%s: %s", name, msg);
		    write(sock, named_msg, strlen(named_msg)); // 형식 맞춰서 서버로 전송
        }
	}
	exit(0);
}

 
///////////////////////////////////////////////////////////////////////
// Function : void *recv_(void *sock_fd)                             //
// ================================================================= //
// Input: x                                                          //
// Output: msg (talk or name_list or exit_msg)                      //
// Purpose: receive message from server                              //
///////////////////////////////////////////////////////////////////////
void *recv_(void *sock_fd) 
{
    int sock = *(int *)sock_fd;
	char msg[BUFSIZE];
	while(1)
	{
		memset((void *)msg, '\0', sizeof(msg));
		recv(sock, msg, BUFSIZE, 0); //서버로 온 메시지를 수신하기 위한 recv
		printf("%s", msg); // 수신받은 메시지 출력
	}
	exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function : int main(int argc, char **argv)                                                      //
// ==============================================================================================  //
// Input: ./[실행파일] [IP] [USERNAME]                                                             //
// Output: x                                                                                      //
// Purpose: send message to server and receive message from server                                //
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int sockfd;
	char buf[BUFSIZE];
	pthread_t send_thread, recv_thread;
	struct sockaddr_in serv_addr;
	/* check argument count */
	if(argc != 3){
		printf("Usage : %s <IP> <name>\n", argv[0]);
		exit(1);
	}
	//인자로 받은 name을 sprintf 함수를 써서 name에 넣어준다.
	sprintf(name, "%s", argv[2]);

	/* open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*initialize server socket information struct to zero and add info*/
	memset((void *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(3500);

	/*connect to server */
	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		perror("connect");

	/* if connect successful, send name info to server */
	write(sockfd, name, sizeof(name));

	/* sending thread, receiving thread production */
	pthread_create(&send_thread, NULL, send_, &sockfd);
	pthread_create(&recv_thread, NULL, recv_, &sockfd);

	/*call join func for successful resource return */
	pthread_join(send_thread, NULL);
	pthread_join(recv_thread, NULL);

	close(sockfd);
	return 0;
}
