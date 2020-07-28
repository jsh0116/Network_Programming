//////////////////////////////////////////////////////////////////////////
// File Name       : chat_server_select.c                               //
// Date            : 2020/06/20 ~ 2020/06/30                            // 
// OS              : Ubuntu 18.04.4 LTS                                 // 
// Student Name    : Seung Hoon Jeong                                   //
// Student ID      : 2015707003                                         //
// -------------------------------------------------------------------  //
// Title : Network Programming Term Project                             //
// Description : Linux 환경 멀티 채팅 서버 및 클라이언트 프로그램 개발      //
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXUSER 1000
#define MAXLINE 1024
#define NAME 20
#define PORTNUM 3500
#define LISTENQ 20

int main () 
{
    int listenfd, connfd;
	int clilen, cli_num = 0;
	int maxfd;
	int fdnum;
	char *ptr; //골뱅이 문자 위치 찾기 위한 포인터
	char welcome_msg[MAXLINE];
	char exit_msg[MAXLINE];
	int client[MAXUSER] = {0};
	char name[MAXUSER][NAME];
	char buf[MAXLINE];
	char buff[MAXLINE];
	char show_list[MAXLINE];
	char login_state[MAXLINE];
	struct sockaddr_in server_addr, client_addr;
	fd_set fds_read, allfds;
	
	/* open socket */
	if((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		
	}
	/* prevent bind error after server terminated */
	int option = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) { 
        perror("setsockopt");
        exit(0);
    }

	/*initialize server socket information struct to zero and add info*/
	memset((void*)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	/* bind socket */
	if(bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		exit(0);
	}

	/* listen queue production */
	if((listen(listenfd, LISTENQ)) == -1) {
		perror("listen");
		exit(0);
	}

	FD_ZERO(&fds_read); //fd_set initialization
	FD_SET(listenfd, &fds_read); //listenfd를 fd_set에 setting
	maxfd = listenfd; //send max value to select()

	while(1) {
		allfds = fds_read; // copy본 store (select 사용전에)
		/*select함수 인자 순서대로, 
		fd 값중 max value, read event check, write check, execpt check, timeout value. NULL infinite wait*/
		fdnum = select(maxfd + 1, &allfds, (fd_set *)0, (fd_set *)0, NULL);
		//when an read event occurs to listenfd , Please accept
		if(FD_ISSET(listenfd, &allfds)) {
			clilen = sizeof(client_addr);
			connfd = accept(listenfd, (struct sockaddr *)&client_addr, &clilen);
			FD_SET(connfd, &fds_read); //accepted socket fd register
			client[connfd] = connfd; //login client storing

			read(connfd, name[connfd], NAME); // receive name sending client to server
			
			strcpy(buff,name[connfd]); //send message to only request client
			strcat(buff,"님 멀티 채팅방에 오신 것을 환영합니다.\n");
			cli_num++; // login user num check
			write(client[connfd],buff, strlen(buff));
			bzero(buff,sizeof(buff));
			//printf("check name : %s\n", name[accept_sock]);
			if(connfd > maxfd) // if accepted socketfd가 기존의 max값보다 크다면 
				maxfd = connfd; // exchange

			sprintf(welcome_msg, "%s님이 채팅방에 입장했습니다.\n", name[connfd]);

			/*send message to rest process(login state) */
			for(int i = 0; i <= maxfd; i++) {
				if(client[i] != client[connfd])
					send(client[i], welcome_msg, sizeof(welcome_msg), 0);
			}

			//printf("[%d] Accept OK\n", accept_sock); // accept check
			continue;
		}

		/* when No accept request */
		for (int check_fd = 0; check_fd <= maxfd; check_fd++) { // loop searching, find to applicable socket fd
			//어떤 fd 값이 select read 이벤트 검사한 곳에서 이벤트 요청한게 확인 됐다면
			if (FD_ISSET(check_fd, &allfds)) {
				bzero(buf,sizeof(buf));
				if((read(check_fd, buf, MAXLINE)) <= 0) {
					/* when other user disconnected */
					close(check_fd);
					FD_CLR(check_fd, &fds_read);
					//if(client_array[check_fd] != client_array[accept_sock])
					//sprintf(exit_msg,"%s님이 대화방을 나갔습니다.\n", name[check_fd]);
					strcpy(exit_msg,name[check_fd]);
					strcat(exit_msg,"님이 대화방을 나갔습니다.\n"); // exit_msg production
					cli_num--; // login user num check
					for(int j=0; j<=maxfd; j++)
						if(client[j] == j)
							send(client[j],exit_msg,sizeof(exit_msg),0); // send exit_msg to all login state client (except own client) 
					bzero(exit_msg,sizeof(exit_msg));
					client[check_fd] = 0;
					memset((void *)name[check_fd], 0x00, sizeof(name[check_fd])); // initialize client's fd and name
				}
			else {
				ptr = strchr(buf, '@'); //if sending message includes '@'가 있다면 return value to ptr applicable location
				if (ptr != NULL)  {
					if(!strcmp(ptr, "@show\n")) { //is msg @show? check
						bzero(show_list,sizeof(show_list));
						strcpy(show_list,"현재 접속중인 사용자는 다음과 같습니다.\n");
						if(client[check_fd] == check_fd);//check fd
						for (int i = 0; i <= maxfd; i++) { //send name to all login state process
							if(name[i][0] != '\0') {
								strcat(show_list,name[i]);
								strcat(show_list," ");
								//strcat(show_list,"현재 접속자 수:");
								//strcat(show_list,cli_num);
							}
						}
						//bzero(login_state,sizeof(login_state));
						sprintf(login_state,"현재 접속자 수:%d\n",cli_num);
						strcat(show_list,login_state);
						write(client[check_fd], show_list, sizeof(show_list));
						
						
					}
				}
				
				//printf("Read : %s" ,buf); //server check ex
				for(int i = 0; i <= maxfd; i++) {
					if(client[i] != 0)
						if(strcmp(buf,"@show\n"))
							send(client[i], buf, sizeof(buf), 0); //send message to all login state client(except own client)
					}
				}
				if(--fdnum <= 0) break;
			}
		}
	}
	close(listenfd);
	return 0;
}
