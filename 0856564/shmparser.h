#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <bitset>
#include <sys/ipc.h>
#include <algorithm>
#include <sys/shm.h>

#include "shell_case3.h"

using namespace std;

struct user{
	int id;
	int socket;
	string name;
	string addr;

	user(){
		id = -1;
		socket = 0 ;
		name = "(no name)";
		addr = "";
	}

	user(int id, int socket, string name, string addr) : id(id),socket(socket), name(name), addr(addr) {

	}
};

struct user_shm{
	int id;
	int socket;
	char name[1234];
	char addr[1234];

	user_shm(){
		id = -1;
		socket = 0 ;
		strcpy(name,"(no name)");
		strcpy(addr, " ");
	}

	// user(int id, int socket, string name, string addr) : id(id),socket(socket), name(name), addr(addr) {

	// }
};

struct msg{
	int type; //tell 2 bb 1 exit 0
	int fd;
	char m[1234];

	msg() {
		type = -1;
		fd = -1;
	}
};



class shmparser {
protected:
	//vector<int> idcntcross;
	vector<user> uservec;
	//vector<vector<int> > socketpipecross;
	//vector<vector<array<int,2> > > socketfdcross;
	user my;
public:
	shell sh;

	static bool cmp(user a, user b ){
		return (a.id < b.id);
	}

	void load_pid() {
		piduserpipe* int_pointer;
		int shm_id;
		if ((shm_id = shmget( 3005 , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
		}
		if ((int_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
	    	perror("shmat");
		}

		pidcross.clear();
		userpipecross.clear();
		for ( int i = 0 ; i < 35 ; i++ ) {
			pidcross.push_back((int_pointer)->pidv[i]);
			userpipecross.push_back((int_pointer)->userpipev[i]);
		}
		
    	shmdt(int_pointer);
	}

	void update_pid() {
		piduserpipe* int_pointer;
		int shm_id;
		if ((shm_id = shmget( 3005 , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
		}
		if ((int_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
	    	perror("shmat");
		}

		for ( int i = 0 ; i < 35 ; i++ ) {
			(int_pointer)->pidv[i] = pidcross[i];
			(int_pointer)->userpipev[i] = userpipecross[i];		
		}
		
    	shmdt(int_pointer);
	}

	void load_user(){

		user_shm* user_pointer;
		int shm_id;
		if ((shm_id = shmget(3000, 30 * sizeof(user_shm), IPC_CREAT | 0666)) < 0) {
	        perror("shmget");
	    }
		if ((user_pointer = (user_shm*)shmat(shm_id, NULL, 0)) == (user_shm *) -1) {
        	perror("shmat");
    	}
		uservec.clear();
		for ( int i = 0 ; i < 30 ; i++ ) {
			if ( (user_pointer+i)->id != -1 ) {
				char tmpname[1234];
				char tmpaddr[1234];
				strcpy(tmpname, (user_pointer+i)->name);
				strcpy(tmpaddr, (user_pointer+i)->addr);
				string sname(tmpname);
				string saddr(tmpaddr);
				user ttt((user_pointer+i)->id,(user_pointer+i)->socket,sname,saddr);
				uservec.push_back(ttt);
			}
		}
		
    	shmdt(user_pointer);
	}

	void update_user() {

		user_shm* user_pointer;
		int shm_id;
		if ((shm_id = shmget(3000, 30 * sizeof(user_shm), IPC_CREAT | 0666)) < 0) {
	        perror("shmget");
	    }
		if ((user_pointer = (user_shm*)shmat(shm_id, NULL, 0)) == (user_shm *) -1) {
        	perror("shmat");
    	}
    	//uservec.clear();
		for ( int i = 0 ; i < 30 ; i++ ) {
			//(user_pointer+i)->name = uservec[i].name ;
			if ( i < uservec.size() ) {
				strncpy((user_pointer+i)->name,uservec[i].name.c_str(),uservec[i].name.size()+1) ;
				(user_pointer+i)->id = uservec[i].id ;
				strncpy((user_pointer+i)->addr,uservec[i].addr.c_str(),uservec[i].addr.size()+1) ;
				//strncpy((user_pointer+i)->socket,uservec[i].socket.c_str(),uservec[i].socket.size()+1) ;
				(user_pointer+i)->socket = uservec[i].socket ;
			} else {
				user_shm tmp ;
				(user_pointer+i)->id = tmp.id ;
				//////cerr << "dsf\n" << endl ;
				strcpy((user_pointer+i)->name , tmp.name );	
				strcpy((user_pointer+i)->addr , tmp.addr );
				(user_pointer+i)->socket = tmp.socket ;
			}
			
		}
		shmdt(user_pointer);
	}

	void load_id() {
		int* int_pointer;
		int shm_id;
		if ((shm_id = shmget( 3001 , 35 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
		}
		if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
	    	perror("shmat");
		}

		idcntcross.clear();
		for ( int i = 0 ; i < 35 ; i++ ) {
			idcntcross.push_back(*(int_pointer+i));
		}
		
    	shmdt(int_pointer);
	}

	void update_id() {
		int* int_pointer;
		int shm_id;
		if ((shm_id = shmget( 3001 , 35 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    	perror("shmget101");	
		}
		if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
	    	perror("shmat");
		}
		for ( int i = 0 ; i < 35 ; i++ ) {
			*(int_pointer+i) = idcntcross[i];
		}
    	shmdt(int_pointer);
	}

	void load_pipe() {
		int shm_id;
		int* int_pointer;
		if ((shm_id = shmget( 3002 , 1225 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    		perror("shmget101");	
		}
		if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
	    	perror("shmat");
		}
		socketpipecross.clear();
		for ( int i = 0 ; i < 35 ; i++ ) {
			vector<int> tt ;
			for ( int j = 0 ; j < 35 ; j++ ) {
				//////cerr << *(int_pointer + ((i*35)+j)) << " " ;
				tt.push_back(*(int_pointer + ((i*35)+j) ));
			}
			//////cerr << endl;
			socketpipecross.push_back(tt);
		}
		shmdt(int_pointer);	
	}

	void update_pipe() {
		int shm_id;
		int* int_pointer;
		if ((shm_id = shmget( 3002 , 1225 * sizeof(int), IPC_CREAT | 0666)) < 0) {
    		perror("shmget101");	
		}
		if ((int_pointer = (int*)shmat(shm_id, NULL, 0)) == (int*) -1) {
	    	perror("shmat");
		}
		for ( int i = 0 ; i < socketpipecross.size() ; i++ ) {
			for ( int j = 0 ; j < socketpipecross[i].size() ; j++ ) {
				*(int_pointer + ((i*35)+j)) = socketpipecross[i][j];
				//////cerr << *(int_pointer + ((i*35)+j)) << " " ;
			}
			//////cerr << endl;
		}
		shmdt(int_pointer);	
	}

	void boardcastusermsg(vector<boardcastmsg> allusermsg) {
		
		for ( auto i : allusermsg ) {
			string ss ;

			switch(i.c) {
				case 0 :
					ss = "*** Error: user #";
					ss += to_string(i.a);
					ss += " does not exist yet. ***\n";
					write(1,ss.c_str(),ss.size());
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
					boardcast(ss);
					break;
				case 2 :
					ss = "*** Error: the pipe #";
					ss += to_string(i.a);
					ss += "->#";
					ss += to_string(i.b);
					ss += " already exists. ***\n";
					write(1,ss.c_str(),ss.size());
					break;
				case 3 :
					ss = "*** ";
					ss += id_find_name(i.b);
					ss += " (#";
					ss += to_string(i.b);
					ss += ") just received from ";
					ss += id_find_name(i.a);
					ss += " (#";
					ss += to_string(i.a);
					ss += ") by '";
					ss += i.cmd;
					ss += "' ***\n";
					boardcast(ss);
					break;
				case 4 :
					ss = "*** Error: the pipe #";
					ss += to_string(i.a);
					ss += "->#";
					ss += to_string(i.b);
					ss += " does not exist yet. ***\n";
					write(1,ss.c_str(),ss.size());
					break;
			} 	
			
		}
	}

	string get_ipport(sockaddr_in addr) {

		char ip_address[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(addr.sin_addr), ip_address, INET_ADDRSTRLEN);
		string ss(ip_address) ;
		ss += ":";
		ss += to_string(htons(addr.sin_port));
		return ss;
	}

	void login(int sk, sockaddr_in addr) {
		string Welcome = "****************************************\n** Welcome to the information server. **\n****************************************\n" ;
		write(sk,Welcome.c_str(),Welcome.size());

		load_pid();
		load_id();
		perror("vdsccc");
		load_user();
		perror("vdsccc");
		//load_pipe();
		//////cerr << uservec.size() << endl;
		perror("vdsccc");
		int id = 0;
		for ( int i = 1 ; i < 32 ; i++ ) {
			if ( idcntcross[i] == 0 ) {
				id = i ;
				idcntcross[i] = 1 ;
				break;
			}
		}
		pidcross[id] = getpid();
		string ss;
		ss = get_ipport(addr);
		user tmpuser(id,sk,"(no name)",ss);

		string sss = "*** User '(no name)' entered from ";
		sss += tmpuser.addr;
		sss += ". ***\n";

		uservec.push_back(tmpuser);
		
		update_pid();
		update_id();
		update_user();
		perror("vdsccc");
		//update_pipe();
		perror("vdsccc");
		boardcast(sss);
		write(sk,"% ",2);
		//who(1);
	}

	void client_login() {
		//load_id();
		load_user();
		//int aaa ;
		//cin >> aaa ;
		//////cerr << aaa << endl;
		my = uservec[uservec.size()-1];
		for ( int i = 0 ; i < uservec.size()-1 ; i++ ) {
			close(uservec[i].socket);
		}
		////cout << ss << endl;
		// update_id();
		// update_user();

		
	};

	void logout() {

		string ss = "*** User '";
		ss += my.name;
		ss += "' left. ***\n";
		boardcast(ss);

		idcntcross[my.id] = 0 ;
		int user_i = id_find_user(my.id);
		uservec.erase(uservec.begin()+user_i);
		for ( int i = 0 ; i < 35 ; i++ ) {
			socketpipecross[i][my.id] = 0 ;
			socketpipecross[my.id][i] = 0 ;
		}

		update_user();
		update_id();
		update_pipe();

		int shm_id;
		msg* msg_pointer;
		key_t key = 3003 ;
		if ((shm_id = shmget( key , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
	    	perror("shmget100");	
		}
		if ((msg_pointer = (msg*)shmat(shm_id, NULL, 0)) == (msg*) -1) {
	    	perror("shmat");
		}
		

		(msg_pointer)->type = 3;
		(msg_pointer)->fd = my.socket;
		//strncpy((msg_pointer)->m,ss.c_str(),ss.size()+1) ;
		//////cerr << (msg_pointer)->m << endl;
		shmdt(msg_pointer);
		kill(father_pid,SIGUSR1);
		close(my.socket);
	}

	void boardcast( string ss ) {
		// for ( int i = 0 ; i < uservec.size() ; i++ ){
		// 	////cout << uservec[i].socket << endl;
		// 	write( uservec[i].socket, ss.c_str(), ss.size() );
		// }
		int shm_id;
		msg* msg_pointer;
		key_t key = 3003 ;
		if ((shm_id = shmget( key , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
	    	perror("shmget100");	
		}
		if ((msg_pointer = (msg*)shmat(shm_id, NULL, 0)) == (msg*) -1) {
	    	perror("shmat");
		}
		
		(msg_pointer)->type = 1;
		strncpy((msg_pointer)->m,ss.c_str(),ss.size()+1) ;
		//////cerr << (msg_pointer)->m << endl;
		
		kill(father_pid,SIGUSR1);
		while((msg_pointer)->type != 0 ){
			;
		}
		shmdt(msg_pointer);
	}
	void boardhandler(int singo) {
		//////cerr << "qqqq" << endl;
		load_user();
		int shm_id;
		//load_user();
		//string ss = "testtesttest\n";

		msg* msg_pointer;
		key_t key = 3003 ;
		if ((shm_id = shmget( key , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
	    	perror("shmget100");	
		}
		if ((msg_pointer = (msg*)shmat(shm_id, NULL, 0)) == (msg*) -1) {
	    	perror("shmat");
		}

		//////cerr << "qqq " << uservec.size() << endl;
		if ( (msg_pointer)->type == 1 ) {
			for ( int i = 0 ; i < uservec.size() ; i++ ){
				// ////cerr << uservec[i].socket << endl;
				// ////cerr << (msg_pointer)->m << endl;
				// ////cerr << "dsc" << endl;
				write( uservec[i].socket , (msg_pointer)->m , strlen((msg_pointer)->m));
			}
			(msg_pointer)->type = 0;
		} else if ( (msg_pointer)->type == 2 ) {
			write( (msg_pointer)->fd , (msg_pointer)->m , strlen((msg_pointer)->m));
		} else if ( (msg_pointer)->type == 3 ) {
			close( (msg_pointer)->fd );
		}
		
		//int status ;
		//while( waitpid(-1,&status,WNOHANG) > 0 );
		
		//////cerr << "fdsfds" << endl;
		shmdt(msg_pointer);
	}


	void userpipehandler(int singo) {
		
		load_pid();
		int shm_id;

		piduserpipe* pid_pointer;
		key_t key = 3005 ;
		if ((shm_id = shmget( key , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
	    	perror("shmget100");	
		}
		if ((pid_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
	    	perror("shmat");
		}

		//////cerr << "qqq " << uservec.size() << endl;
		int in_id = (pid_pointer)->userpipev[my.id];
		string ss = sh.user_pipe_name(in_id,my.id); 
		openfd[in_id] = open(ss.c_str(), O_RDONLY );

		shmdt(pid_pointer);
	}
	// void start_boardhandler() {
	// 	signal(SIGUSR1,boardhandler);
	// }

	void who( int fd ) {
		vector<user> tmpuservec(uservec.begin(),uservec.end());
		sort(tmpuservec.begin(),tmpuservec.end(),cmp);

		string sstitle = "<ID>	<nickname>	<IP:port>	<indicate me>\n"; 
		write(fd,sstitle.c_str(),sstitle.size());
		for ( int i = 0 ; i < tmpuservec.size() ; i++ ) {
			string ss ;
			ss += to_string(tmpuservec[i].id) ;
			ss += "	";
			ss += tmpuservec[i].name ;
			ss += "	";
			ss += tmpuservec[i].addr;
			ss += "	";

			//ss += 
			if ( tmpuservec[i].id == my.id ) {
				ss += "<-me\n";
			}else ss += "\n";

			write(fd,ss.c_str(),ss.size());
		}
	}

	void tell( string name, string msgg, int fd) {
		string ss = "*** ";
		ss += name;
		ss += " told you ***: ";
		ss += msgg;
		ss += "\n";
		//write(fd,ss.c_str(),ss.size());

		int shm_id;
		msg* msg_pointer;
		key_t key = 3003 ;
		if ((shm_id = shmget( key , sizeof(struct msg), IPC_CREAT | 0666)) < 0) {
	    	perror("shmget100");	
		}
		if ((msg_pointer = (msg*)shmat(shm_id, NULL, 0)) == (msg*) -1) {
	    	perror("shmat");
		}
		
		(msg_pointer)->type = 2;
		(msg_pointer)->fd = fd;
		strncpy((msg_pointer)->m,ss.c_str(),ss.size()+1) ;
		//////cerr << (msg_pointer)->m << endl;
		shmdt(msg_pointer);
		kill(father_pid,SIGUSR1);
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

	int id_find_user( int id ) {
		for ( int i = 0 ; i < uservec.size() ; i++ ) {
			if ( uservec[i].id == id ) {
				return i ;
			}
		}
		return -1 ;
	}

	string id_find_name( int id ) {
		for ( int i = 0 ; i < uservec.size() ; i++ ) {
			if ( uservec[i].id == id ) {
				return uservec[i].name;
			}
		}
		return "" ;
	}

	void buildinrun() {
		//write(my.socket,"test",4);
		int sk = 1 ;
		if ( sh.who() ) {
			who(1);
		}
		if ( sh.yell() ) {
			yell(sh.get_yell(),my.name);
		}
		if ( sh.name() ) {
			bool bb = false;
			string tmpname = sh.get_name();
			for ( int j = 0 ; j < uservec.size() ; j++ ) {
				if ( uservec[j].name == tmpname ) {
					bb = true;
					break;
				}
			}
			
			if ( bb == false) {
				string ss = "*** User from ";
				ss += my.addr;
				ss += " is named '";
				ss += tmpname;
				ss += "'. ***\n";
				my.name = tmpname;
				int user_i = id_find_user(my.id);
				uservec[user_i].name = tmpname;
				boardcast(ss);
			}else {
				string ss = "*** User '";
				ss += tmpname;
				ss += "' already exists. ***\n";
				write(sk,ss.c_str(),ss.size());
			}
		}
		if ( sh.tell() ) {
			int id = sh.get_tell_id();
			if ( idcntcross[id] == 0 ){
				string ss = "*** Error: user #";
				ss += to_string(id);
				ss += " does not exist yet. ***\n";
				write(1,ss.c_str(),ss.size());
			}else {
				int user_i = id_find_user(id);
				tell( my.name , sh.get_tell() , uservec[user_i].socket );
			}
		}

	}

	void update_mytouservec() {
		int user_i = id_find_user(my.id);
		uservec[user_i].name = my.name;
		uservec[user_i].addr = my.addr;
		uservec[user_i].socket = my.socket;
		uservec[user_i].id = my.id;
	}

	int get_id() {
		return my.id;
	}

};