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
#include "shell.h"

using namespace std;

class user {
	
protected:
	int user_fd;
	int id;
	sockaddr_in addr;

	string name;
	shell sh;

public:
	user() {
		name = "(noname)";

	}

	void user_goshell(){
		while(1){
			sh.start_shell();		
			if ( sh.exit() ) break;	
			if ( sh.name() ) { 
				name = sh.get_name();
				cout << name << endl;
			}
			//cout << "??";
		}	
	}

	string get_name() {
		return name;
	}

	int get_id() {
		return id ;
	}

	string get_ipport() {

		
		char ip_address[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(addr.sin_addr), ip_address, INET_ADDRSTRLEN);
		//fprintf(stderr, "connected from %s:%u\n", ip_address, client_addr.sin_port);
		string ss(ip_address) ;
		ss += to_string(addr.sin_port);
		return ss;
	}




};