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

using namespace std; 

void test(int fd) {
	for ( auto i = 0 ; i < 10 ; i++ ) {
		char cc[4] = "123" ;
		string ss = "tsetse";
		//if (write(fd, cc, 3) == -1) perror("send");	
		cout << cc ;
		cin >> cc ;
		cout << ss ;
	}
	
}


int main () {
	
	int port = 8889 ;

	sockaddr_in socketadd ;

	int xiaocheui = socket(AF_INET, SOCK_STREAM, 0);

	socketadd.sin_family = AF_INET;
	socketadd.sin_port = htons(port);
	socketadd.sin_addr.s_addr = INADDR_ANY;

	int yes = 1 ;
	if (setsockopt(xiaocheui,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
  		perror("setsockopt");
  		exit(1);
	}

	if ( bind(xiaocheui, (struct sockaddr *)&socketadd, sizeof socketadd) == -1 ) {
		fprintf(stderr,"fail bind\n");
	}

	if ( listen(xiaocheui,1) == -1 ) {
		fprintf(stderr,"fail listen\n");
	}
	
	while(1) {
		sockaddr_in client_addr;
		unsigned addrlen = sizeof(client_addr);
		int fd = accept(xiaocheui, (struct sockaddr *)&client_addr, &addrlen);
		if ( fd == -1 ) continue ;
		if ( !fork() ) {
			close(xiaocheui);
			dup2(fd,STDIN_FILENO);
			dup2(fd,STDERR_FILENO);
			dup2(fd,STDOUT_FILENO);
			//char test123[123];
			//if (send(fd, "Hello, world!", 13, 0) == -1) perror("send");
			//if (write(fd, "Hello, world!", 13) == -1) perror("send");
			//if (read(fd,test123,123) == -1 )perror("read");
			//cout << test123 << endl ;
			char ip_address[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(client_addr.sin_addr), ip_address, INET_ADDRSTRLEN);
			fprintf(stderr, "connected from %s:%u\n", ip_address, client_addr.sin_port);
			if ( execl("npshell","npshell",NULL) == -1 ) cout << "fail\n";
			//test(fd);
        	close(fd);
		}else {
			close(fd);
		}

	}

}
