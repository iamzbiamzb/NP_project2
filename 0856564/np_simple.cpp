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
#include "shell_case1.h"
#include <bitset>

using namespace std; 

// int status ;

// void childhandler(int singo) {
// 	//int status ;
// 	while( waitpid(-1,&status,WNOHANG) > 0 );
// }

void masterchildhandler(int singo) {
	int status ;
	while( waitpid(-1,&status,WNOHANG) > 0 );
}

int main(int argc , char** argv) {
	
	signal(SIGCHLD,masterchildhandler);
	string portss(argv[1]);
	int port = stoi(portss) ;

	sockaddr_in socketadd ;

	int master_socket = socket(AF_INET, SOCK_STREAM, 0);

	socketadd.sin_family = AF_INET;
	socketadd.sin_port = htons(port);
	socketadd.sin_addr.s_addr = INADDR_ANY;

	int yes = 1 ;
	if (setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof (yes)) == -1) {
  		perror("setsockopt");
  		exit(1);
	}

	if ( bind(master_socket, (struct sockaddr *)&socketadd, sizeof socketadd) == -1 ) {
		fprintf(stderr,"fail bind\n");
	}

	if ( listen(master_socket,1) == -1 ) {
		fprintf(stderr,"fail listen\n");
	}
	
	while(1) {

		sockaddr_in client_addr;
		unsigned addrlen = sizeof(client_addr);
		int fd = accept(master_socket, (struct sockaddr *)&client_addr, &addrlen);
		if ( fd == -1 ) continue ;
		
		char ip_address[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(client_addr.sin_addr), ip_address, INET_ADDRSTRLEN);
		fprintf(stderr, "connected from %s:%u\n", ip_address, client_addr.sin_port);

		pid_t pid ;
		pid = fork();
		if ( pid == 0 ) {
			dup2(fd,STDIN_FILENO);
			dup2(fd,STDOUT_FILENO);
			dup2(fd,STDERR_FILENO);
			close(master_socket);
			shell one(fd,fd,fd);
			one.start_shell();

		}else {
			close(fd);	
			wait(NULL);
		}

		
    	
    	

    	//cout << "??" << endl ;
	}

}