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
#include <set>
#include <string>
#include <bitset>

using namespace std;

class user_man {

protected:
	
	//int idcnt[35];
	vector<vector<int> > socketpipecross;
	//vector<int> idcnt;
	set<string> nameset;

public:
	vector<user> user_vec;

	user_man() {
		//idcnt.resize(35,0);
		//pipecnt.resize(35,vector<int>(35,0));
	}

	void boardcast( string ss ) {
		//cout << ss << endl;
		for ( int i = 0 ; i < user_vec.size() ; i++ ){
			write( user_vec[i].get_user_fd(), ss.c_str(), ss.size() );
		}
	}

	void boardcastusermsg(vector<boardcastmsg> allusermsg) {
		
		for ( auto i : allusermsg ) {
			string ss ;

			switch(i.c) {
				case 0 :
					ss = "*** Error: user #";
					ss += to_string(i.a);
					ss += " does not exist yet. ***\n";
					break;
				case 1 :
					ss = "*** ";
					ss += id_find_name(i.a);
					ss += " (#";
					ss += to_string(i.a);
					ss += ") just piped '";
					ss += i.cmd;
					ss += "' to ";
					ss += id_find_name(i.b);
					ss += " (#";
					ss += to_string(i.b);
					ss += ") ***\n";
					break;
				case 2 :
					ss = "*** Error: the pipe #";
					ss += to_string(i.a);
					ss += "->#";
					ss += to_string(i.b);
					ss += " already exists. ***\n";
					break;
				case 3 :
					ss = "*** ";
					ss += id_find_name(i.a);
					ss += " (#";
					ss += to_string(i.a);
					ss += ") just received from ";
					ss += id_find_name(i.b);
					ss += " (#";
					ss += to_string(i.b);
					ss += ") by '";
					ss += i.cmd;
					ss += "' ***\n";
					break;
				case 4 :
					ss = "*** Error: the pipe #";
					ss += to_string(i.b);
					ss += "->#";
					ss += to_string(i.a);
					ss += " does not exist yet. ***\n";
					break;
			} 	
			boardcast(ss);
		}
	}

	void who( int id , int fd ) {
		string sstitle = "<ID>	<nickname>	<IP:port>	<indicate me>\n"; 
		write(fd,sstitle.c_str(),sstitle.size());
		for ( int i = 0 ; i < user_vec.size() ; i++ ) {
			string ss ;
			ss += to_string(user_vec[i].get_id()) ;
			ss += "	";
			ss += user_vec[i].get_name() ;
			ss += "	";
			ss += user_vec[i].get_ipport();
			ss += "	";

			//ss += 
			if ( user_vec[i].get_id() == id ) {
				ss += "<-me\n";
			}else ss += "\n";

			write(fd,ss.c_str(),ss.size());
		}
	}

	void tell( string name, string msg, int fd) {
		string ss = "*** ";
		ss += name;
		ss += " told you ***: ";
		ss += msg;
		ss += "\n";
		write(fd,ss.c_str(),ss.size());
	}

	void name( string ipport, string name ) {
		string ss = "*** User from ";
		ss += ipport ;
		ss += " is named '";
		ss += name ;
		ss += "'. ***\n";
		boardcast(ss);
	}

	void yell( string ss, string name ) {
		string tmpss = "*** ";
		tmpss += name ;
		tmpss += " yelled ***: ";
		tmpss += ss;
		tmpss += "\n";
		boardcast(tmpss);
	}

	void user_login( sockaddr_in addr, int socketfd) {
		int id = 0;
		for ( int i = 1 ; i < 32 ; i++ ) {
			if ( idcntcross[i] == 0 ) {
				id = i ;
				idcntcross[i] = 1 ;
				break;
			}
		}

		user user(addr,socketfd,id);
		user_vec.push_back(user);
		string ss = "*** User '(no name)' entered from ";
		ss += user.get_ipport();
		ss += ".\n";
		boardcast(ss);
	}

	void user_logout( int user_i ) {
		cout << "user_i: " << user_i << user_vec[user_i].get_ipport() << "\n" ;
		string ss = "*** User '";
		ss += user_vec[user_i].get_name();
		ss += "' left. ***\n" ;
		boardcast(ss);
		
		idcntcross[user_vec[user_i].get_id()] = 0 ;
		auto it = nameset.find(user_vec[user_i].get_name());
		if ( it != nameset.end() ) nameset.erase(it);
		user_vec.erase(user_vec.begin()+user_i);

		//cout << "out\n";
	}

	int socket_find_user( int socketfd ) {
		for ( int i = 0 ; i < user_vec.size() ; i++ ) {
			if ( user_vec[i].get_user_fd() == socketfd ) {
				return i ;
			}
		}
		return -1 ;
	}

	string id_find_name( int id ) {
		for ( int i = 0 ; i < user_vec.size() ; i++ ) {
			if ( user_vec[i].get_id() == id ) {
				return user_vec[i].get_name() ;
			}
		}
		return "" ;
	}

	int id_find_user( int id ) {
		for ( int i = 0 ; i < user_vec.size() ; i++ ) {
			if ( user_vec[i].get_id() == id ) {
				return i ;
			}
		}
		return -1 ;
	}

	void user_run( int i ) {
		user_vec[i].user_goshell();
	}

	void buildinrun( int i , int sk ) {
		if ( user_vec[i].sh.who() ) {
			who(user_vec[i].get_id(),sk);
		}
		if ( user_vec[i].sh.yell() ) {
			yell(user_vec[i].sh.get_yell(),user_vec[i].get_name());
		}
		if ( user_vec[i].sh.name() ) {
			auto it = nameset.find(user_vec[i].sh.get_name());
			if ( it == nameset.end() ) {
				string ss = "*** User from ";
				ss += user_vec[i].get_ipport();
				ss += " is named '";
				ss += user_vec[i].sh.get_name();
				ss += "'. ***\n";
				nameset.erase(user_vec[i].get_name());
				user_vec[i].set_name(user_vec[i].sh.get_name());

				nameset.insert(user_vec[i].sh.get_name());
				boardcast(ss);
			}else {
				string ss = "*** User '";
				ss += user_vec[i].sh.get_name();
				ss += "' already exists. ***\n";
				write(sk,ss.c_str(),ss.size());
			}
		}
		if ( user_vec[i].sh.tell() ) {
			int id = user_vec[i].sh.get_tell_id();
			if ( idcntcross[id] == 0 ){
				string ss = "*** Error: user #";
				ss += id;
				ss += " does not exist yet. ***\n";
				write(sk,ss.c_str(),ss.size());
			}else {
				int user_i = id_find_user(id);
				tell( user_vec[i].get_name() , user_vec[i].sh.get_tell() , user_vec[user_i].get_user_fd() );
			}
		}

	}


};