#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <bitset>
#include "user_man.h"

using namespace std; 

// int status ;

// void childhandler(int singo) {
// 	//int status ;
// 	while( waitpid(-1,&status,WNOHANG) > 0 );
// }

int main(int argc, char** argv) {
	
	

	string portss(argv[1]);
	int port = stoi(portss) ;

	sockaddr_in socketadd ;

	int master_socket = socket(AF_INET, SOCK_STREAM, 0);

	socketadd.sin_family = AF_INET;
	socketadd.sin_port = htons(port);
	socketadd.sin_addr.s_addr = INADDR_ANY;

	int yes = 1 ;
	if (setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
  		perror("setsockopt");
  		exit(1);
	}

	if ( bind(master_socket, (struct sockaddr *)&socketadd, sizeof socketadd) == -1 ) {
		fprintf(stderr,"fail bind\n");
	}

	if ( listen(master_socket,1) == -1 ) {
		fprintf(stderr,"fail listen\n");
	}
	
	int max_socket = master_socket;
	fd_set masterfds_, readfds_;
	FD_ZERO(&masterfds_);
	FD_ZERO(&readfds_);
	FD_SET(master_socket, &masterfds_);

	user_man user_man;
	
	while(1) {
		readfds_ = masterfds_;
		if (select(max_socket+1, &readfds_, NULL, NULL, NULL) == -1) {
			continue;
		}
		for(int i=0; i<=max_socket; i++) {
			if (FD_ISSET(i, &readfds_)) {
				if (i == master_socket) {
					sockaddr_in client_addr;
					unsigned addrlen = sizeof(client_addr);
					int client_socket = accept(master_socket, (sockaddr*) &client_addr, &addrlen);
					FD_SET(client_socket, &masterfds_);
					max_socket = max(max_socket, client_socket);
					string ss = "****************************************\n** Welcome to the information server. **\n****************************************\n" ;

					write(client_socket,ss.c_str(),ss.size());
					user_man.user_login(client_addr, client_socket);
					write(client_socket,"% ",2);
				}else {
					int client_socket = i ;
					int user_i = user_man.socket_find_user(client_socket);
					char command[15005];
					user_man.user_vec[user_i].setuserenv();
					user_man.user_vec[user_i].sh.reset_flag();
					user_man.user_vec[user_i].sh.clearusermsg();
					user_man.user_vec[user_i].sh.set_sk(client_socket,client_socket,client_socket);	
					current_id = user_man.user_vec[user_i].get_id();
					int k = user_man.user_vec[user_i].sh.input_command(client_socket,command);
					user_man.user_vec[user_i].sh.command_strtok(command);
					user_man.user_vec[user_i].sh.parser();	
					vector<boardcastmsg> allusermsg = user_man.user_vec[user_i].sh.getusermsg();
					user_man.boardcastusermsg(allusermsg,user_man.user_vec[user_i].get_user_fd());				
					user_man.user_vec[user_i].sh.run_command();
					user_man.buildinrun(user_i,client_socket);
					if ( user_man.user_vec[user_i].sh.exit() ){
						
						close(client_socket);
						FD_CLR(client_socket, &masterfds_);
						//cout << "whree\n" ; 
						user_man.user_logout(user_i);
					}else {
						write(client_socket,"% ",2);	
					}
					
				}
			}
		}







		//cout << "ds "; 
		// sockaddr_in client_addr;
		// unsigned addrlen = sizeof(client_addr);
		// int fd = accept(master_socket, (struct sockaddr *)&client_addr, &addrlen);
		// if ( fd == -1 ) continue ;
		
		// char ip_address[INET_ADDRSTRLEN];
		// inet_ntop(AF_INET, &(client_addr.sin_addr), ip_address, INET_ADDRSTRLEN);
		// fprintf(stderr, "connected from %s:%u\n", ip_address, client_addr.sin_port);
		//if ( execl("npshell","npshell",NULL) == -1 ) cout << "fail\n";
		//test(fd);
		//shell one(fd,fd,fd);
		//one.start_shell();
		//send(fd,"hello",5,0);
    	
    	//close(fd);

    	//cout << "??" << endl ;
	}

}