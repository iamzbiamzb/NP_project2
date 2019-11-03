#include "user.h"
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

using namespace std;

class user_man {

protected:
	vector<user> user_vec;

public:

	void boardcast( string ss ) {
		//cout << ss << endl;
	}

	void who( int id ) {
		boardcast( "<ID>	<nickname>	<IP:port>	<indicate me>" );
		// for ( int i = 0 ; i < user_vec.size() ; i++ ) {
		// 	string ss ;
		// 	ss += to_string(user_vec[i].get_id()) ;
		// 	ss += "	";
		// 	ss += user_vec[i].get_name() ;
		// 	ss += "	";
		// 	ss += user_vec[i].get_ipport();
		// 	ss += "	";

		// 	//ss += 
		// 	if ( user_vec[i].get_id() == id ) {
		// 		ss += "<-me";
		// 	}
		// 	boardcast(ss);
		// }
	}

	void tell() {

	}

	void yell() {

	}

	void user_login( user user ) {
		

		//user user_tmp;
		user_vec.push_back(user);
	}

	void user_run( int i ) {
		user_vec[i].user_goshell();
	}


};