#include <unistd.h>   
#include <cstdlib>  
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/wait.h>   
#include <signal.h>     
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <array>
#include <fstream>
#include <fcntl.h>


//#include "user.h"

using namespace std;

//int status;

#define BUFSIZE 15005

extern pid_t father_pid;
extern vector<vector<int> > socketpipecross;
extern vector<int> idcntcross;

extern vector<int> pidcross;
extern vector<int> userpipecross;
extern vector<int> openfd;

struct boardcastmsg{
	int a;
	int b;
	int c;
	string cmd;
};

struct piduserpipe{
	int pidv[35]; //tell 2 bb 1 exit 0
	int userpipev[35];
};

vector<vector<int> > socketpipecross(35,vector<int>(35,0));
vector<int> idcntcross(35,0); 
vector<int> pidcross(35,0);
vector<int> userpipecross(35,0);
vector<int> openfd(35,0);

class shell{

protected:
	struct pipe_list {
		vector<string> line ;
		string cmd ;
		int cnt ;
		int type ;
		string filename ; 
		pipe_list* next ;
		bool fail_flag;
		array<int,2> in_fd;
		array<int,2> out_fd;
		vector<pipe_list*> pre ;
		pipe_list() : fail_flag(false) {}
	};	
//bilin flag
	bool who_flag;
	bool yell_flag;
	bool name_flag;
	bool tell_flag;
	bool in_flag;
	bool out_flag;

	string nickname;
	string yell_msg;
	string tell_msg;
	int tell_id;
	//int status ;	
	string filename ;
	vector<pipe_list*> pipe_vec ;
	vector<string> string_args ;

	string envpath ;
	bool exit_flag ;
	int threeFD[3];
	vector<boardcastmsg> usermsg;
	//char buf[BUF_SIZE+1];

public:

	shell(int a, int b, int c ) : who_flag(false), yell_flag(false), name_flag(false), tell_flag(false), in_flag(false), out_flag(false) {
		signal(SIGCHLD,childhandler);
		exit_flag = false ;
		setenv("PATH","bin:.",1);
		envpath = "bin:.";
		threeFD[0] = a;
		threeFD[1] = b;
		threeFD[2] = c;
	}

	shell() : who_flag(false), yell_flag(false), name_flag(false), tell_flag(false), in_flag(false), out_flag(false) {
		signal(SIGCHLD,childhandler);
		exit_flag = false ;
		setenv("PATH","bin:.",1);
		envpath = "bin:.";
		threeFD[0] = STDIN_FILENO;
		threeFD[1] = STDOUT_FILENO;
		threeFD[2] = STDERR_FILENO;
	}

	string user_pipe_name(int i, int j) {
		string ss("./user_pipe/") ;
		ss += to_string(i);
		ss += "_to_";
		ss += to_string(j);	
		return ss;
	}

	static void childhandler(int singo) {
		int status ;
		while( waitpid(-1,&status,WNOHANG) > 0 );
	}

	vector<boardcastmsg> getusermsg(){
		return usermsg;
	}

	void clearusermsg() {
		usermsg.clear();
	}

	string turn_line_to_cmd(vector<string> ss) {
		string sss;
		for ( auto i : ss ) {
			sss += i;
			sss += " ";
		}
		sss.erase(sss.end()-1);
		return sss;
	}

	void makeusermsg(int a, int b, int c, string ss){
		boardcastmsg tmp;
		tmp.a = a;
		tmp.b = b;
		tmp.c = c;
		string sss ;
		
		tmp.cmd = ss;
		usermsg.push_back(tmp);
	}

	void pipe_without_err( string cmd , vector<string > line , array<int,2> p_in_arr , int* p_out , int flag , int type ) {
		
		char** cmd_vec = new char*[line.size()+1];
	    int ii = 0 ;
		for ( ii = 0 ; ii < line.size() ; ii++ ) {
			char* tmp_char = new char[line[ii].length()] ;
			strcpy(tmp_char,line[ii].c_str());
			cmd_vec[ii] = tmp_char;
			////cout << cmd_vec[ii] ;//<< endl;
			// if ( !strcmp(cmd_vec[ii], "-1" ) )//cout << "???-1" << endl;
			// if ( !strcmp(cmd_vec[ii], "-n" ) )//cout << "???-n" << endl;
			// if ( !strcmp(cmd_vec[ii], "ls" ) )//cout << "???ls" << endl;
			// if ( !strcmp(cmd_vec[ii], "cat" ) )//cout << "???cat" << endl;
		}
		cmd_vec[ii] = NULL ;

// 0 |
// 1 !
// 2 noremal
// 3 '>'
// 4 >
// 5 <
// 6 < >
// 7 < '>'
// < | 8
// < ! 9
		// //cout << "tt: " << type << endl;
		// //cout << "ff: " << flag << endl;
		int p_in[2] ;
		p_in[0] = p_in_arr[0];
		p_in[1] = p_in_arr[1];
		int pid ;
		int fileno , fileno2; 
		////cerr << "flag: " << flag << endl ;
		switch(flag) {
			case 0 :
				if ( type == 8 || type == 9 ) {
					//cout << p_in[0] << " " << p_in[1] << endl;
					//string ss(user_pipe_name(p_in[0],p_in[1]));
					//fileno2 = open(ss.c_str(), O_RDONLY );
					fileno2 = openfd[p_in[0]];
				}

				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					if ( type == 1 || type == 9 ) dup2(p_out[1],STDERR_FILENO);
					else dup2(threeFD[2], STDERR_FILENO);

					if ( type == 8 || type == 9 ){
						
						dup2(fileno2,STDIN_FILENO);
					}

					dup2(p_out[1],STDOUT_FILENO);
					close(p_out[0]);
					//close(p_in[0]);
					//close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						::exit(0);
					} 
				}else {
					if ( type == 8 || type == 9 ) {
						close(fileno2);
					}
					
					//close(p_in[0]);
					//close(p_in[1]);
					//waitpid((pid_t)pid, &status, 0);

				}
				break;
			case 1 :
				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					if ( type == 1 ) dup2(p_out[1],STDERR_FILENO);
					else dup2(threeFD[2], STDERR_FILENO);
					dup2(p_out[1],STDOUT_FILENO);
					close(p_out[0]);
					dup2(p_in[0],STDIN_FILENO);
					close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						::exit(0);
					}
				}else {
					close(p_in[0]);
					close(p_in[1]);
					//waitpid((pid_t)pid, &status, 0);

				}
				break;
			case 2 :
				
				if ( type == 3 ) fileno = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666,0);
				
				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					dup2(threeFD[2], STDERR_FILENO);
					if ( type == 3 ) dup2(fileno, STDOUT_FILENO);
					else if ( type == 4 ) {
						string ss(user_pipe_name(p_out[0],p_out[1]));
						unlink(ss.c_str());
						while ( mkfifo(ss.c_str(),0666) < 0 ) {
							usleep(1000);
						}
						int shm_id;
						piduserpipe* pid_pointer;
						key_t key = 3005 ;
						if ((shm_id = shmget( key , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
					    	perror("shmget100");	
						}
						if ((pid_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
					    	perror("shmat");
						}
						(pid_pointer)->userpipev[p_out[1]] = p_out[0]; 
						shmdt(pid_pointer);
						kill(pidcross[p_out[1]],SIGUSR2);
						fileno = open(ss.c_str(), O_WRONLY );
						dup2(fileno,STDOUT_FILENO);
					}else {
						dup2(threeFD[1],STDOUT_FILENO);
						dup2(threeFD[2],STDERR_FILENO);
					}
					
					dup2(p_in[0],STDIN_FILENO);
					close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						::exit(0);
					}
				}else {
					
					close(p_in[0]);
					close(p_in[1]);
					int status;
					if ( type != 4 ) waitpid((pid_t)pid, &status, 0);
					if ( type == 3 ) close(fileno);
				}
				break;
			case 3 :
				//int fileno ; 
				if ( type == 3 || type == 7 ) fileno = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666,0);
				
				if ( type == 5 || type == 6 || type == 7 ) {
					fileno2 = openfd[p_in[0]];
				}
				while ( (pid = fork()) < 0 ) {
					//cerr << "fdsfs"  << endl;
					usleep(1000);
				}
				//cerr << "fdsfsvdsv"  << endl;
				if ( pid == 0 ) {
					dup2(threeFD[2], STDERR_FILENO);
					if ( type == 3 ) dup2(fileno, STDOUT_FILENO);
					else if ( type == 4 ) {
						string ss(user_pipe_name(p_out[0],p_out[1]));
						unlink(ss.c_str());
						if (mkfifo(ss.c_str(),0666) < 0) {
							//cerr << "fifo" << endl;
						}
						int shm_id;
						piduserpipe* pid_pointer;
						key_t key = 3005 ;
						if ((shm_id = shmget( key , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
					    	perror("shmget100");	
						}
						if ((pid_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
					    	perror("shmat");
						}
						(pid_pointer)->userpipev[p_out[1]] = p_out[0]; 
						shmdt(pid_pointer);
						kill(pidcross[p_out[1]],SIGUSR2);
						fileno = open(ss.c_str(), O_WRONLY );
						dup2(fileno,STDOUT_FILENO);
					} else if ( type == 5 ) {
						dup2(fileno2,STDIN_FILENO);
					} else if ( type == 6 ) {
						string ss(user_pipe_name(p_out[0],p_out[1]));
						unlink(ss.c_str());
						if (mkfifo(ss.c_str(),0666) < 0) {
							//cerr << "fifo" << endl;
						}
						int shm_id;
						piduserpipe* pid_pointer;
						key_t key = 3005 ;
						if ((shm_id = shmget( key , sizeof(struct piduserpipe), IPC_CREAT | 0666)) < 0) {
					    	perror("shmget100");	
						}
						if ((pid_pointer = (piduserpipe*)shmat(shm_id, NULL, 0)) == (piduserpipe*) -1) {
					    	perror("shmat");
						}
						(pid_pointer)->userpipev[p_out[1]] = p_out[0]; 
						shmdt(pid_pointer);
						kill(pidcross[p_out[1]],SIGUSR2);
						fileno = open(ss.c_str(), O_WRONLY );
						dup2(fileno,STDOUT_FILENO);
						dup2(fileno2,STDIN_FILENO);
					}else if ( type == 7 ) {
						dup2(fileno,STDOUT_FILENO);
						dup2(fileno2,STDIN_FILENO);
					}else {
						dup2(threeFD[1],STDOUT_FILENO);
						dup2(threeFD[2],STDERR_FILENO);
					}

					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						::exit(0);
					} 
				}else {
					
					//if ( type == 4 || type == 6 ) close(fileno);
					
					//close(p_in[1]);
					//close(p_in[0]);
					int status;
					//cerr << "vsvd" << endl ;
					
					//cerr << "vvvv" << endl;
					
					if ( type != 4 && type != 6 ) waitpid((pid_t)pid, &status, 0);
					if ( type == 3 || type == 7 ) close(fileno);
					if ( type == 5 || type == 6 || type == 7 ){
						close(fileno2);
					}
				}
				break;
		}
			
	}

	void run_command() {
		for ( auto i = pipe_vec.begin() ; i != pipe_vec.end() ;){
	  		if ( (*i)->cnt == 0 ) {
	      		
	  			if ( (*i)->cmd == "exit" ){
	  				exit_flag = true ;
	  				break;
	  			}else if ( (*i)->cmd == "printenv" ) {
	  				char* tmpg = getenv((*i)->line[1].c_str());
	  				////cout << getenv((*i)->line[1].c_str()) << endl << flush;
	  				write(threeFD[1],tmpg,strlen(tmpg));
	  				write(threeFD[1],"\n",1);
	  			}else if ( (*i)->cmd == "setenv" ) {
	  				setenv((*i)->line[1].c_str(),(*i)->line[2].c_str(),1);
	  				if ( (*i)->line[1] == "PATH" ){
	  					envpath = (*i)->line[2];
	  				}
	  			}else if ( (*i)->cmd == "yell" ) {
	  				yell_flag = true;
	  				yell_msg = (*i)->line[1];
	  				for ( int z = 2 ; z < (*i)->line.size() ; z++ ) {
	  					yell_msg += " ";
	  					yell_msg += (*i)->line[z];
	  				}
	  			}else if ( (*i)->cmd == "name" ) {
	  				name_flag = true;
	  				nickname = (*i)->line[1];
	  			}else if ( (*i)->cmd == "who" ) {
	  				who_flag = true;
	  			}else if ( (*i)->cmd == "tell" ) {
	  				tell_flag = true;
	  				
	  				tell_id = stoi((*i)->line[1]);
	  				tell_msg = (*i)->line[2];
	  				for ( int z = 3 ; z < (*i)->line.size() ; z++ ) {
	  					tell_msg += " ";
	  					tell_msg += (*i)->line[z];
	  				}
	  			}else if ( (*i)->fail_flag == true ) {
	  				;
	  			}else {
					int pp[2] ;
					array<int,2> arr;	
					arr = run((*i),pp,false);
	  			}

	  			// if ( (*i)->fail_flag == true ) //cout << "true" << endl;
	  			// else //cout << "false" << endl;
	  			
	      		pipe_vec.erase(i);
	      	}else {
	      		i++;
	      	}
	  	}
	}

	array<int,2> run( pipe_list* it , int* p_out , bool fir ) {
		//test((*it)->cmd,(*it)->line);
		int p[2] ;
		array<int,2> pa;
		//int stt = pipe(p);

		if( it->pre.size() != 0 ) {

			for ( auto i : it->pre ) {
				if ( i == it->pre[0] ) {
					////cout << it->pre[0] << endl ;
					pa = run(i,p,true);
				}else {
					p[0] = pa[0];
					p[1] = pa[1];
					pa = run(i,p,false);
				}
			}
		}
		// three condition
		// 0 = first
		// 1 = mid
		// 2 = last
		// 3 = only one
		int st ;
		if ( fir ) {
			while( (st = pipe(p)) < 0 ) {
				////cerr << "???" << endl;
				usleep(1000);
			}
		} else {
			p[0] = p_out[0];
			p[1] = p_out[1];
		}
		int flag = 2 ;
		if ( it->pre.size() == 0 && it->next == NULL  ) flag = 3 ;
		else if ( it->next == NULL ) flag = 2 ;
		else if ( it->pre.size() == 0 ) flag = 0 ;
		else flag = 1 ;

		if ( it->type == 3 ) filename = it->filename ; 	
		if ( it->type == 7 ) filename = it->filename ; 
		if ( it->type == 4 ){
			p[0] = it->out_fd[0];
			p[1] = it->out_fd[1];
		}else if ( it->type == 5 ){
			pa[0] = it->in_fd[0];
			pa[1] = it->in_fd[1];
		}else if ( it->type == 6 ){
			pa[0] = it->in_fd[0];
			pa[1] = it->in_fd[1];
			p[0] = it->out_fd[0];
			p[1] = it->out_fd[1];
		}else if ( it->type == 7 ){
			pa[0] = it->in_fd[0];
			pa[1] = it->in_fd[1];
		}else if ( it->type == 8 ){
			pa[0] = it->in_fd[0];
			pa[1] = it->in_fd[1];
		}else if ( it->type == 9 ){
			pa[0] = it->in_fd[0];
			pa[1] = it->in_fd[1];
		}

		// << "type: " << it->type << endl ;
		////cerr << pa[0] << " " << pa[1] << " " << p[0] << " " << p[1] << endl;
		pipe_without_err(it->cmd,it->line,pa,p,flag,it->type);

		pa[0] = p[0];
		pa[1] = p[1];
		return pa ;
		
	}	

	int input_command(int fd, char* command) {
		int len = read(fd, command, BUFSIZE - 1);
		command[len] = '\0';
		while (len && command[len-1] < 32) {
			command[--len] = '\0';
		}
		return strlen(command);
	}
	
	void command_strtok( char* command ) {
		vector<char*> args;
    	char* prog = strtok( command, " " );
    	char* tmp = prog;
    	while ( tmp != NULL )
    	{
      		args.push_back( tmp );
      		tmp = strtok( NULL, " " );
      	}

      	
      	string_args.clear();
      	for (auto i : args) {
      		string tmp_string(i);
      		string_args.push_back(tmp_string);
      	}

		if ( string_args[string_args.size()-1].back() == '\r' ) string_args[string_args.size()-1].pop_back();
		if ( string_args[string_args.size()-1].back() == '\n' ) string_args[string_args.size()-1].pop_back();
		if ( string_args[string_args.size()-1].back() == '\r' ) string_args[string_args.size()-1].pop_back();
	}


	void parser_helper( int type, int pipe_cnt, vector<string> line, string cmd, string filename, array<int,2> in_fd, array<int,2> out_fd, bool fail_flag) {
		pipe_list* tmp_pipe_list = new pipe_list ;
      	tmp_pipe_list->next = NULL ;
      	tmp_pipe_list->type = type ;
      	tmp_pipe_list->filename = filename ;
      	tmp_pipe_list->fail_flag = fail_flag ;
      	tmp_pipe_list->out_fd = out_fd ;
      	tmp_pipe_list->in_fd = in_fd ;

      	for ( auto j : line ) {
			tmp_pipe_list->line.push_back(j);
		}
		tmp_pipe_list->cmd = cmd ;
		tmp_pipe_list->cnt = pipe_cnt ;
		for ( auto k = pipe_vec.begin() ; k != pipe_vec.end() ; ){
			(*k)->cnt = (*k)->cnt -1 ;
			if ( (*k)->cnt == 0 ) {
				(*k)->next = tmp_pipe_list ;
				tmp_pipe_list->pre.push_back((*k));
				pipe_vec.erase(k);
			}else {
				k++;
			}
		}
		pipe_vec.push_back(tmp_pipe_list);
		//cout << tmp_pipe_list->type << endl;

	}

	void parser(int user_i) {
		vector<string> line ;
		vector<string> tmpsrgs(string_args.begin(),string_args.end()) ;
      	string cmd ;
      	array<int,2> ii;
      	array<int,2> oo;
      	//cout << user_i << endl;
      	for (int i = 0 ; i < string_args.size() ; i++ ) {
      		if ( line.empty() ) cmd = string_args[i] ;

      		if ( string_args[0] == "tell" ) {
      			vector<string> ss(string_args.begin()+2,string_args.end());
      			line.push_back(string_args[0]);
      			line.push_back(string_args[1]);
      			line.push_back(turn_line_to_cmd(ss));
      			////cout << cmd << " "<< line[0] << " " << line[1] << " " << line[2] << endl;
      			int pipe_cnt = 0 ;
      			parser_helper(2,pipe_cnt,line,cmd,"1",ii,oo,false);
      			line.clear();
      			break;
      		}

      		if ( string_args[0] == "yell" ) {
      			vector<string> ss(string_args.begin()+1,string_args.end());
      			line.push_back(string_args[0]);
      			line.push_back(turn_line_to_cmd(ss));
      			////cout << cmd << " " << line[0] << " " << line[1] << endl;
      			int pipe_cnt = 0 ;
      			parser_helper(2,pipe_cnt,line,cmd,"1",ii,oo,false);
      			line.clear();
      			break;
      		}


      		if ( string_args[i][0] == '|' ) {

      			int pipe_cnt = 0 ;
      			if ( string_args[i].length() == 1 ) {
      				pipe_cnt = 1 ;
      			}else {
      				string_args[i].erase(string_args[i].begin());
      				pipe_cnt = stoi(string_args[i]);
      			}
      			parser_helper(0,pipe_cnt,line,cmd,"1",ii,oo,false);
      			line.clear();

      		}else if ( string_args[i][0] == '!' ) {

      			int pipe_cnt = 0 ;
      			if ( string_args[i].length() == 1 ) {
      				pipe_cnt = 1 ;
      			}else {
      				string_args[i].erase(string_args[i].begin());
      				pipe_cnt = stoi(string_args[i]);
      			}
      			parser_helper(1,pipe_cnt,line,cmd,"1",ii,oo,false);
      			line.clear();

      		}else if( string_args[i][0] == '>' && string_args[i].size() == 1 ) {
      			
      			int pipe_cnt = 0 ;
      			parser_helper(3,pipe_cnt,line,cmd,string_args[i+1],ii,oo,false);
      			line.clear();
      			i++ ;
      			///////////////////////////////////////////////////////////////////////////////////////////////////
      			///////////////////////////////////////////////////////////////////////////////////////////////////
      			///////////////////////////////////////////////////////////////////////////////////////////////////
      		}else if( string_args[i][0] == '>' ) {
      			string_args[i].erase(string_args[i].begin());
      			int out_cnt = stoi(string_args[i]);

      			if ( i == string_args.size()-1 ) {
      				if ( idcntcross[out_cnt] == 1 && out_cnt != user_i && socketpipecross[user_i][out_cnt] == 0 ) {
	      				//ok
	      				socketpipecross[user_i][out_cnt] = 1 ;
	      				
	      				int pipe_cnt = 0;

	      				oo[0] = user_i;
	      				oo[1] = out_cnt;
	     
	      				parser_helper(4,pipe_cnt,line,cmd,"1",ii,oo,false);
	      				makeusermsg(user_i,out_cnt,1,turn_line_to_cmd(tmpsrgs));//send 
	      				line.clear();
	      				
	      			}else {
	      				//false
	      				if ( idcntcross[out_cnt] == 0 ) makeusermsg(out_cnt,out_cnt,0,""); //userid
	      				else if ( socketpipecross[user_i][out_cnt] == 1 ) makeusermsg(user_i,out_cnt,2,""); //exist
	      				
	      				int pipe_cnt = 0;
	      				parser_helper(4,pipe_cnt,line,cmd,"1",ii,oo,true);
	      				line.clear();
	      			}
      			}else if ( string_args[i+1][0] == '<' && string_args[i+1].size() == 1 ) {

      			}else if ( string_args[i+1][0] == '<' ) {
      				string_args[i+1].erase(string_args[i+1].begin());
      				int in_cnt = stoi(string_args[i+1]);
	      			
	      			if ( idcntcross[in_cnt] == 1 && socketpipecross[in_cnt][user_i] == 1 && out_cnt != user_i && in_cnt != user_i \
	      				&& idcntcross[out_cnt] == 1 && socketpipecross[user_i][out_cnt] == 0 ) {
	      				// ok
	      				socketpipecross[user_i][out_cnt] = 1 ;
	      				socketpipecross[in_cnt][user_i] = 0;
	      				
	      				int pipe_cnt = 0 ;
	      			
	      				ii[0] = in_cnt;
	      				ii[1] = user_i;
	      				oo[0] = user_i;
	      				oo[1] = out_cnt;
	      				//oo[1] = pp[1];
	      				parser_helper(6,pipe_cnt,line,cmd,"1",ii,oo,false);
	      				
	      				string ss(">");
	      				ss += string_args[i];
	      				line.push_back(ss);

	      				ss = "<";
	      				ss += string_args[i+1];
	      				line.push_back(ss);
	      				
	      				i++;
	      				makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
	      				makeusermsg(user_i,out_cnt,1,turn_line_to_cmd(tmpsrgs));//send 
	      				
	      				line.clear();
	      			}else {
	      				//false
	      				if ( idcntcross[out_cnt] == 0 ) makeusermsg(out_cnt,out_cnt,0,""); //userid
	      				else if ( socketpipecross[user_i][out_cnt] == 1 ) makeusermsg(user_i,out_cnt,2,""); //exist
	      				if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
	      				else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
	      				
	      				
	      				int pipe_cnt = 0;
	      				parser_helper(6,pipe_cnt,line,cmd,"1",ii,oo,true);
	      				line.clear();
	      				i++;
	      			}
      			}


      		}else if( string_args[i][0] == '<' ) {
      			string_args[i].erase(string_args[i].begin());
      			int in_cnt = stoi(string_args[i]);

      			if ( i == string_args.size()-1 ) {
      				if ( idcntcross[in_cnt] == 1 && in_cnt != user_i && socketpipecross[in_cnt][user_i] == 1 ) {
      					// ok
      					socketpipecross[in_cnt][user_i] = 0;
      					int pipe_cnt = 0;
      					ii[0] = in_cnt;
      					ii[1] = user_i;
      					parser_helper(5,pipe_cnt,line,cmd,"1",ii,oo,false);
	      				
      					string ss("<");
	      				ss += string_args[i];
	      				line.push_back(ss);

	      				makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
	      				line.clear();
      				}else {
      					// fail
      					if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
      					else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
      					
      					int pipe_cnt = 0;
      					parser_helper(5,pipe_cnt,line,cmd,"1",ii,oo,true);
	      				line.clear();
      				}
      			}else if ( string_args[i+1][0] == '>' ) {
      				if ( string_args[i+1].size() == 1 ) {
      					// >> type 7 
      					if ( idcntcross[in_cnt] == 1 && in_cnt != user_i && socketpipecross[in_cnt][user_i] == 1 ) {
      						//ok
      						socketpipecross[in_cnt][user_i] = 0;
      						int pipe_cnt = 0;
      						ii[0] = in_cnt;
      						ii[1] = user_i;
      						parser_helper(7,pipe_cnt,line,cmd,string_args[i+2],ii,oo,false);
      						
	      					string ss("<");
	      					ss += string_args[i];
	      					line.push_back(ss);

	      					ss = ">";
	      					line.push_back(ss);

	      					ss = string_args[i+2];
	      					line.push_back(ss);

	      					makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
	      					line.clear();
      						i++;
      						i++;
      					}else {
      						//fail
      						if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
      						else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
      						
      						int pipe_cnt = 0;
      						parser_helper(7,pipe_cnt,line,cmd,string_args[i+2],ii,oo,true);
	      					line.clear();
	      					i++;
      						i++;
      					}

      				}else {
      					// normal
      					string_args[i+1].erase(string_args[i+1].begin());
      					int out_cnt = stoi(string_args[i+1]);

      					if ( idcntcross[in_cnt] == 1 && socketpipecross[in_cnt][user_i] == 1 && in_cnt != user_i && out_cnt != user_i \
		      				&& idcntcross[out_cnt] == 1 && socketpipecross[user_i][out_cnt] == 0 ) {
		      				// ok
		      				socketpipecross[in_cnt][user_i] = 0;
		      				socketpipecross[user_i][out_cnt] = 1 ;
		      				
		      				int pipe_cnt= 0 ;

		      				ii[0] = in_cnt;
		      				ii[1] = user_i;
		      				oo[0] = user_i;
		      				oo[1] = out_cnt;
		      				//oo[1] = pp[1];
		      				parser_helper(6,pipe_cnt,line,cmd,"1",ii,oo,false);
		      				
		      				string ss("<");
	      					ss += string_args[i];
	      					line.push_back(ss);

	      					ss = ">";
	      					ss += string_args[i+1];
	      					line.push_back(ss);

		      				makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
		      				makeusermsg(user_i,out_cnt,1,turn_line_to_cmd(tmpsrgs));//send 
		      				line.clear();
		      				i++;
	      					
		      			}else {
		      				//false
		      				int pipe_cnt = 0;
      						parser_helper(6,pipe_cnt,line,cmd,"1",ii,oo,true);
	      					line.clear();
	      					i++;
	      					if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
	      					else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
	      					if ( idcntcross[out_cnt] == 0 ) makeusermsg(out_cnt,out_cnt,0,""); //userid
	      					else if ( socketpipecross[user_i][out_cnt] == 1 ) makeusermsg(user_i,out_cnt,2,""); //exist
		      			}
      				}
      			}else if ( string_args[i+1][0] == '|' ) {
      				int pipe_cnt = 0 ;
	      			if ( string_args[i+1].length() == 1 ) {
	      				string_args[i+1].erase(string_args[i+1].begin());
	      				pipe_cnt = 1 ;
	      			}else {
	      				string_args[i+1].erase(string_args[i+1].begin());
	      				pipe_cnt = stoi(string_args[i+1]);
	      			}

	      			if ( idcntcross[in_cnt] == 1 && in_cnt != user_i && socketpipecross[in_cnt][user_i] == 1 ) {
	      				socketpipecross[in_cnt][user_i] = 0;
      					ii[0] = in_cnt;
      					ii[1] = user_i;
      					parser_helper(8,pipe_cnt,line,cmd,"1",ii,oo,false);

      					string ss("<");
      					ss += string_args[i];
      					line.push_back(ss);

      					ss = "|";
      					ss += string_args[i+1];
      					line.push_back(ss);

	      				makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
	      				line.clear();
	      				i++;
	      				
      				}else {
      					if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
      					else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
      					
      					i++;
      				}
      				
      			}else if ( string_args[i+1][0] == '!' ) {
      				int pipe_cnt = 0 ;
	      			if ( string_args[i+1].length() == 1 ) {
	      				string_args[i+1].erase(string_args[i+1].begin());
	      				pipe_cnt = 1 ;
	      			}else {
	      				string_args[i+1].erase(string_args[i+1].begin());
	      				pipe_cnt = stoi(string_args[i+1]);
	      			}
      				
      				if ( idcntcross[in_cnt] == 1 && in_cnt != user_i && socketpipecross[in_cnt][user_i] == 1 ) {	
      					socketpipecross[in_cnt][user_i] = 0;
      					ii[0] = in_cnt;
      					ii[1] = user_i;
      					parser_helper(9,pipe_cnt,line,cmd,"1",ii,oo,false);

      					string ss("<");
      					ss += string_args[i];
      					line.push_back(ss);

      					ss = "!";
      					ss += string_args[i+1];
      					line.push_back(ss);

      					makeusermsg(in_cnt,user_i,3,turn_line_to_cmd(tmpsrgs));//recv 
	      				line.clear();
      					i++;
      					
      				}else {
      					if ( idcntcross[in_cnt] == 0 ) makeusermsg(in_cnt,in_cnt,0,""); //userid
      					else if ( socketpipecross[in_cnt][user_i] == 0 ) makeusermsg(in_cnt,user_i,4,""); //dont exist
      					
      					i++;
      				}
      				
      			}
      		///////////////////////////////////////////////////////////////////////////////////////////////////
  			///////////////////////////////////////////////////////////////////////////////////////////////////
  			///////////////////////////////////////////////////////////////////////////////////////////////////
      		}else if( i == string_args.size()-1 ) {
      			line.push_back(string_args[i]);
      			int pipe_cnt = 0 ;
      			parser_helper(2,pipe_cnt,line,cmd,"1",ii,oo,false);
      			line.clear();
      		}else {
      			line.push_back(string_args[i]);
      		}


      	}
	}
// 0 |
// 1 !
// 2 noremal
// 3 '>'
// 4 >
// 5 <
// 6 < >
// 7 < '>'
// < | 8
// < ! 9
	void reset_flag() {
		who_flag = false;
		yell_flag = false;
		name_flag = false;
		tell_flag = false;
		in_flag = false;
		out_flag = false;
	}

	// void start_shell(){
	// 	write(threeFD[1],"% ",2);
	// 	// read cmd line
	// 	reset_flag();
	// 	char command[15005] ;
	// 	//cin.getline( command, 15005 );
	// 	//fgets(command,15005,stdin);
	// 	int k = input_command(threeFD[0], command);
 //    	command_strtok(command);
 //    	parser();
	//       	// tpye
	//       	// 0 : pipe
	//       	// 1 : ! 
	//       	// 2 : normal
	//       	// 3 : >
 //      	run_command();
	// }
// bool func
	string getuserenv() {
		return envpath;
	}

	bool exit(){
		if ( exit_flag == true ) return true;
		else return false;
	}

	bool yell(){
		if ( yell_flag == true ) return true;
		else return false;
	}
	string get_yell() {
		return yell_msg;
	}

	bool name(){
		if ( name_flag == true ) return true;
		else return false;
	}
	string get_name() {
		return nickname;
	}

	bool tell(){
		if ( tell_flag == true ) return true;
		else return false;
	}
	string get_tell() {
		return tell_msg;
	}
	int get_tell_id() {
		return tell_id;
	}

	bool who(){
		if ( who_flag == true ) return true;
		else return false;
	}

	bool pipe_out(){
		if ( out_flag == true ) return true;
		else return false;
	}

	bool pipe_in(){
		if ( in_flag == true ) return true;
		else return false;
	}
	void set_sk(int a, int b, int c){
		threeFD[0] = a;
		threeFD[1] = b;
		threeFD[2] = c;
	}

};


