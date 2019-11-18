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
#include <functional>
#include <string>
#include <bitset>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "startshell.h"

using namespace std;

void masterchildhandler(int singo) {
	int status ;
	while( waitpid(-1,&status,WNOHANG) > 0 );
}

void delshmhandler(int singo) {
	key_t key = 3000 ;
	int shm_id;
	if ((shm_id = shmget( key , 30 * sizeof(struct user_shm), IPC_CREAT | 0666)) < 0) {
    	perror("shmget100");	
	}
	shmctl(shm_id, IPC_RMID, NULL) ;
	key = 3001;
	if ((shm_id = shmget( key , 35 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
	}
	shmctl(shm_id, IPC_RMID, NULL) ;
	key = 3002 ;
	if ((shm_id = shmget( key , 1225 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
	}
	shmctl(shm_id, IPC_RMID, NULL) ;
	if ((shm_id = shmget( 3003 , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
		perror("shmget100");	
	}
	shmctl(shm_id, IPC_RMID, NULL) ;
	if ((shm_id = shmget( 3005 , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
	}
	shmctl(shm_id, IPC_RMID, NULL) ;
	::exit(0);
}

void shmcreate(){
	int shm_id;
	user_shm* user_pointer;
	int* int_pointer;
//////////
	key_t key = 3000 ;
	if ((shm_id = shmget( key , 30 * sizeof(struct user_shm), IPC_CREAT | 0666)) < 0) {
    	perror("shmget100");	
	}
	if ((user_pointer = (user_shm*)shmat(shm_id, NULL, 0)) == (user_shm*) -1) {
    	perror("shmat");
	}
	for ( int i = 0 ; i < 30 ; i++ ) {
		user_shm tmp ;
		(user_pointer+i)->id = tmp.id ;
		strcpy((user_pointer+i)->name , tmp.name );
		strcpy((user_pointer+i)->addr , tmp.addr );
		(user_pointer+i)->socket = tmp.socket ;
	}
	////cerr << "fdsf"  << endl;
	shmdt(user_pointer);
///////////
	key = 3001;
	if ((shm_id = shmget( key , 35 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
	}
	if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
    	perror("shmat");
	}
	vector<int> tmpid(35,0);
	for ( int i = 0 ; i < 35 ; i++ ) {
		*(int_pointer+i) = tmpid[i];
		//cerr << *(int_pointer+i) << " ";
	}
	shmdt(int_pointer);
// ///////////
	key = 3002 ;
	if ((shm_id = shmget( key , 1225 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
	}
	if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
    	perror("shmat");
	}
	for ( int i = 0 ; i < 1225 ; i++ ) {
		*(int_pointer+i) = 0;
	}
	shmdt(int_pointer);


	msg* msg_pointer;
	key = 3003 ;
	if ((shm_id = shmget( key , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
    	perror("shmget100");	
	}
	if ((msg_pointer = (msg*)shmat(shm_id, NULL, 0)) == (msg*) -1) {
    	perror("shmat");
	}
	
	shmdt(msg_pointer);
}

function<void(int)> callback_wrapper;
void callback_function(int value) {
	callback_wrapper(value);
}

pid_t father_pid = getpid();

int main(int argc, char** argv) {

	string portss(argv[1]);
	int port = stoi(portss);


	signal(SIGINT,delshmhandler);
	signal(SIGCHLD,masterchildhandler);
	shmparser shmm;
	//shmm.start_boardhandler();
	callback_wrapper = bind1st(mem_fun(&shmparser::boardhandler),&shmm);
	struct sigaction sigHandler;
	sigHandler.sa_handler = callback_function;
	sigHandler.sa_flags = SA_NODEFER;
	sigaction(SIGUSR1,&sigHandler,NULL);

	shmcreate();

	// for ( int i = 1 ; i < 31 ; i++ ) {
	// 	for ( int j = 1 ; j < 31 ; j++ ) {
	// 		string ss = shm.sh.user_pipe_name(i,j);
	// 		unlink(ss.c_str());
 //    		mkfifo(ss.c_str(),0666);
	// 	}
	// }

	setenv("PATH","bin:.",1);

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

	
	

	//cerr << "fdsf" << endl;

	while(1) {
		sockaddr_in client_addr;
		unsigned addrlen = sizeof(client_addr);
		int client_socket = accept(master_socket, (sockaddr*) &client_addr, &addrlen);
		if ( client_socket < 0 ) continue;

		pid_t pid = fork();
		if ( pid == 0 ) {
			shmm.login(client_socket,client_addr);
			dup2(client_socket,STDIN_FILENO);
			dup2(client_socket,STDOUT_FILENO);
			dup2(client_socket,STDERR_FILENO);
			close(master_socket);
			start_shell();
			::exit(0);
		}else {
			;
		}
	}
		// while(1) {
		// 	sockaddr_in client_addr;
		// 	unsigned addrlen = sizeof(client_addr);
		// 	int client_socket = accept(master_socket, (sockaddr*) &client_addr, &addrlen);
		// 	 continue;
		// 	//cout << "client_socket: " << client_socket << "\n";
		// }

}