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

using namespace std;

//int status;

#define BUFSIZE 15005


class shell{

protected:
	struct pipe_list {
		vector<string> line ;
		string cmd ;
		int cnt ;
		int type ;
		array<int,2> in_fd;
		array<int,2> out_fd;
		string filename ;
		pipe_list* next ;
		vector<pipe_list*> pre ;
		bool done_flag;
		bool first_flag;

		pipe_list() {
			done_flag = false;
			first_flag = false;
		}
	};

	//int status ;
	string filename ;
	vector<pipe_list*> pipe_vec ;
	bool exit_flag ;
	int threeFD[3];
	//char buf[BUF_SIZE+1];

public:
	
	shell(int a, int b, int c ) {
		signal(SIGCHLD,childhandler);
		exit_flag = false ;
		setenv("PATH","bin:.",1);
		threeFD[0] = a;
		threeFD[1] = b;
		threeFD[2] = c;
	}

	shell() {
		signal(SIGCHLD,childhandler);
		exit_flag = false ;
		setenv("PATH","bin:.",1);
		threeFD[0] = STDIN_FILENO;
		threeFD[1] = STDOUT_FILENO;
		threeFD[2] = STDERR_FILENO;
	}

	static void childhandler(int singo) {
		int status ;
		while( waitpid(-1,&status,WNOHANG) > 0 );
	}

	void pipe_without_err( string cmd , vector<string > line , array<int,2> p_in_arr , array<int,2> p_out_arr , int flag , int type ) {
		
		char** cmd_vec = new char*[line.size()+1];
	    int ii = 0 ;
		for ( ii = 0 ; ii < line.size() ; ii++ ) {
			char* tmp_char = new char[line[ii].length()] ;
			strcpy(tmp_char,line[ii].c_str());
			cmd_vec[ii] = tmp_char;
		}
		cmd_vec[ii] = NULL ;


		int p_in[2] ;
		p_in[0] = p_in_arr[0];
		p_in[1] = p_in_arr[1];
		int p_out[2] ;
		p_out[0] = p_out_arr[0];
		p_out[1] = p_out_arr[1];

		int pid ;
		int fileno ; 
		//cerr << "flag: " << flag << endl ;
		switch(flag) {
			case 0 :
				
				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					if ( type == 1 ) dup2(p_out[1],STDERR_FILENO);
					dup2(p_out[1],STDOUT_FILENO);
					close(p_out[0]);
					//close(p_in[0]);
					//close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						exit(0);
					} 
				}else {
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
					dup2(p_out[1],STDOUT_FILENO);
					close(p_out[0]);
					dup2(p_in[0],STDIN_FILENO);
					close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						exit(0);
					}
				}else {
					close(p_in[0]);
					close(p_in[1]);
					//waitpid((pid_t)pid, &status, 0);
					
				}
				break;
			case 2 :
				
				if ( type == 3 ) fileno = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					if ( type == 3 ) dup2(fileno, STDOUT_FILENO);
					else {
						dup2(threeFD[1],STDOUT_FILENO);
						dup2(threeFD[2],STDERR_FILENO);
					}
					
					//close(p_out[1]);
					//close(p_out[0]);
					dup2(p_in[0],STDIN_FILENO);
					close(p_in[1]);
					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						exit(0);
					}
				}else {
					if ( type == 3 ) close(fileno);
					close(p_in[0]);
					close(p_in[1]);
					int status;
					waitpid((pid_t)pid, &status, 0);

				}
				break;
			case 3 :
				//int fileno ; 
				if ( type == 3 ) fileno = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
				while ( (pid = fork()) < 0 ) {
					usleep(1000);
				}
				if ( pid == 0 ) {
					if ( type == 3 ) dup2(fileno, STDOUT_FILENO);
					else {
						dup2(threeFD[1],STDOUT_FILENO);
						dup2(threeFD[2],STDERR_FILENO);
					}

					int err = execvp(cmd.c_str(),cmd_vec);
					if ( err == -1 ) {
						fprintf(stderr, "Unknown command: [%s].\n",cmd.c_str());
						exit(0);
					} 
				}else {
					if ( type == 3 ) close(fileno);
					//close(p_in[1]);
					//close(p_in[0]);
					int status;
					waitpid((pid_t)pid, &status, 0);
				}
				break;
		}
			
	}

	// array<int,2> run( pipe_list* it , int* p_out , bool fir ) {
	// 	//test((*it)->cmd,(*it)->line);
	// 	int p[2] ;
	// 	array<int,2> pa;
	// 	//int stt = pipe(p);

	// 	if( it->pre.size() != 0 ) {

	// 		for ( auto i : it->pre ) {
	// 			if ( i == it->pre[0] ) {
	// 				//cout << it->pre[0] << endl ;
	// 				pa = run(i,p,true);
	// 			}else {
	// 				p[0] = pa[0];
	// 				p[1] = pa[1];
	// 				pa = run(i,p,false);
	// 			}
	// 		}
	// 	}
	// 	// three condition
	// 	// 0 = first
	// 	// 1 = mid
	// 	// 2 = last
	// 	// 3 = only one
	// 	int st ;
	// 	if ( fir ) {
	// 		while( (st = pipe(p)) < 0 ) {
	// 			cerr << "???" << endl;
	// 			usleep(1000);
	// 		}
	// 	} else {
	// 		p[0] = p_out[0];
	// 		p[1] = p_out[1];
	// 	}
	// 	int flag = 2 ;
	// 	if ( it->pre.size() == 0 && it->next == NULL  ) flag = 3 ;
	// 	else if ( it->next == NULL ) flag = 2 ;
	// 	else if ( it->pre.size() == 0 ) flag = 0 ;
	// 	else flag = 1 ;

	// 	if ( it->type == 3 ) filename = it->filename ; 	

	// 	// << "type: " << it->type << endl ;
	// 	//cerr << pa[0] << " " << pa[1] << " " << p[0] << " " << p[1] << endl;
	// 	pipe_without_err(it->cmd,it->line,pa,p,flag,it->type);

	// 	pa[0] = p[0];
	// 	pa[1] = p[1];
	// 	return pa ;
		
	// }

	void run( pipe_list* it ) {

		
		if ( it->done_flag == true ) return;

		it->done_flag = true;

		if ( it->pre.size() != 0 ) {
			for ( auto i : it->pre ){
				run(i);
			}
		}

		int p[2];
		if ( it->next != NULL && it->next->first_flag == false ) {
			while(pipe(p) < 0) {
				usleep(1000);
			}
			it->next->first_flag = true;
			it->next->in_fd[0] = p[0];
			it->next->in_fd[1] = p[1];
			it->out_fd[0] = p[0];
			it->out_fd[1] = p[1];
		}else if ( it->next != NULL ) {
			it->out_fd[0] = it->next->in_fd[0];
			it->out_fd[1] = it->next->in_fd[1];
		}
		
		if ( it->type == 3 ) filename = it->filename ; 

		int flag;
		if ( it->pre.size() == 0 && it->next == NULL  ) flag = 3 ;
		else if ( it->next == NULL ) flag = 2 ;
		else if ( it->pre.size() == 0 ) flag = 0 ;
		else flag = 1 ;

		pipe_without_err(it->cmd,it->line,it->in_fd,it->out_fd,flag,it->type);		
	}

	char* input_command(int fd, char* command) {
		int len = read(fd, command, BUFSIZE - 1);
		command[len] = '\0';
		while (len && command[len-1] < 32) {
			command[--len] = '\0';
		}
		return command;
	}
	
	void parser_helper( int type, int pipe_cnt, vector<string> line, string cmd, string filename, array<int,2> in_fd, array<int,2> out_fd, bool fail_flag) {
		pipe_list* tmp_pipe_list = new pipe_list ;
      	tmp_pipe_list->next = NULL ;
      	tmp_pipe_list->type = type ;
      	tmp_pipe_list->filename = filename ;
      	//tmp_pipe_list->fail_flag = fail_flag ;
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
				tmp_pipe_list->in_fd = (*k)->next->in_fd;
				(*k)->next = tmp_pipe_list ;
				tmp_pipe_list->pre.push_back((*k));
				pipe_vec.erase(k);
			}else {
				k++;
			}
		}
		if ( type == 0 || type == 1 ){
			bool tt = false;
			for ( auto k = pipe_vec.begin() ; k != pipe_vec.end() ; ){
				if ( (*k)->cnt == pipe_cnt ) {
					tt = true;
					tmp_pipe_list->next = (*k)->next;
					break;
				}
			}

			if ( tt == false ) {
				pipe_list* tp = new pipe_list;
				tmp_pipe_list->next = tp;
			}
		}
		pipe_vec.push_back(tmp_pipe_list);
		//cout << tmp_pipe_list->type << endl;

	}

	void start_shell(){
		//dup2(threeFD[0],STDIN_FILENO);
		//dup2(threeFD[1],STDOUT_FILENO);
		//dup2(threeFD[2],STDERR_FILENO);
		while(true) {
			//cout << "% " << flush;
			write(threeFD[1],"% ",2);
			pid_t pid ;
			// read cmd line
			

			char command[15005] ;
			//cin.getline( command, 15005 );
			//fgets(command,15005,stdin);
			input_command(threeFD[0], command);
	    	vector<char*> args;
	    	char* prog = strtok( command, " " );
	    	char* tmp = prog;
	    	while ( tmp != NULL )
	    	{
	      		args.push_back( tmp );
	      		tmp = strtok( NULL, " " );
	      	}

	      	vector<string> string_args ;

	      	for (auto i : args) {
	      		string tmp_string(i);
	      		string_args.push_back(tmp_string);
	      	}

			if ( string_args[string_args.size()-1].back() == '\r' ) string_args[string_args.size()-1].pop_back();
			if ( string_args[string_args.size()-1].back() == '\n' ) string_args[string_args.size()-1].pop_back();
			if ( string_args[string_args.size()-1].back() == '\r' ) string_args[string_args.size()-1].pop_back();
	// cut to piece

	      	// tpye
	      	// 0 : pipe
	      	// 1 : ! 
	      	// 2 : normal
	      	// 3 : >
	      	array<int,2> ii;
      		array<int,2> oo;
	      	vector<string> line ;
	      	string cmd ;
	      	for (int i = 0 ; i < string_args.size() ; i++ ) {
	      		if ( line.empty() ) cmd = string_args[i] ;
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

	      		}else if( string_args[i][0] == '>' ) {
	      			int pipe_cnt = 0 ;
	      			parser_helper(3,pipe_cnt,line,cmd,string_args[i+1],ii,oo,false);
	      			line.clear();
	      			i++ ;
	      		}else if( i == string_args.size()-1 ) {
	      			line.push_back(string_args[i]);
	      			int pipe_cnt = 0 ;
	      			parser_helper(2,pipe_cnt,line,cmd,"1",ii,oo,false);
	      			line.clear();

	      		}else {
	      			line.push_back(string_args[i]);
	      		}
	      	}

	// run command

	      	for ( auto i = pipe_vec.begin() ; i != pipe_vec.end() ;){
	      		if ( (*i)->cnt == 0 ) {
		      		
	      			if ( (*i)->cmd == "exit" ){
	      				exit_flag = true ;
	      				break;
	      			}else if ( (*i)->cmd == "printenv" ) {
	      				char* tmpg = getenv((*i)->line[1].c_str());
	      				//cout << getenv((*i)->line[1].c_str()) << endl << flush;
	      				write(threeFD[1],tmpg,strlen(tmpg));
	      				write(threeFD[1],"\n",1);
	      			}else if ( (*i)->cmd == "setenv" ) {
	      				setenv((*i)->line[1].c_str(),(*i)->line[2].c_str(),1);
	      			}else {
						run((*i));
	      			}

		      		pipe_vec.erase(i);
		      	}else {
		      		run((*i));
		      		i++;
		      	}
	      	}
	      	if ( exit_flag ){
	      		::exit(0);
	      	} 
		}
		//close(threeFD[0]);
		//close(threeFD[1]);
		//close(threeFD[2]);
	}



};


