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

using namespace std;

struct pipe_list {
	vector<char*> line ;
	char* cmd ;
	int cnt ;
	pipe_list* next ;
	vector<pipe_list*> pre ;
};

struct stat buf ;

int main() {
	
	vector<pipe_list*> pipe_vec ;

	while(true) {

		cout << "% ";
		
		string tmp_cmd ;
		pid_t pid ;
		int* status;

// read command

		char command[10000] ;
		cin.getline( command, 10000 );

    	vector<char*> args;
    	char* prog = strtok( command, " " );
    	char* tmp = prog;
    	while ( tmp != NULL )
    	{
      		args.push_back( tmp );
      		tmp = strtok( NULL, " " );
      	}
      	vector<char*> tmp_vec ;
      	char* tmp_char ;
      	for ( int i = 0 ; i < args.size() ; i++  ){
      		char s1 = '|' ; char s2 = '>' ;
      		if ( tmp_vec.empty() ) tmp_char = args[i] ;
      		if ( args[i][0] == s1 ){
      			pipe_list* tmp_pipe_list = new pipe_list ;

      			int pipe_int = 0 ;
      			if ( strlen(args[i]) == 1 ){
      				pipe_int = 1 ;
      			}else {
      				char tt[] = "" ;
      				const char* tmp_args = &args[i][1] ;
      				strcpy(tt, tmp_args) ;
      				pipe_int = atoi(tt);
      			}
      			
      			for ( auto k : tmp_vec ) {
      				tmp_pipe_list->line.push_back(k);
      			}
      			tmp_pipe_list->cmd = tmp_char ;
      			tmp_pipe_list->cnt = pipe_int ;

      			tmp_vec.clear();
      			

      			for ( auto j = pipe_vec.begin() ; j != pipe_vec.end(); ) {
      				(*j)->cnt = (*j)->cnt -1 ;
      				if ( (*j)->cnt == 0 ){
      					(*j)->next = tmp_pipe_list ;
      					tmp_pipe_list->pre.push_back((*j));
      					pipe_vec.erase(j);
      				}else {
      					j++ ;
      				}
      			}
      			pipe_vec.push_back(tmp_pipe_list);

      		}else if( args[i][0] == s2 ) {		
				pipe_list* tmp_pipe_list = new pipe_list ;
				int pipe_int = 1 ;
				for ( auto k : tmp_vec ) {
      				tmp_pipe_list->line.push_back(k);
      			}
      			tmp_pipe_list->cmd = tmp_char ;
      			tmp_pipe_list->cnt = pipe_int ;

      			tmp_vec.clear();
      			

      			for ( auto j = pipe_vec.begin() ; j != pipe_vec.end(); ) {
      				(*j)->cnt = (*j)->cnt -1 ;
      				if ( (*j)->cnt == 0 ){
      					(*j)->next = tmp_pipe_list ;
      					tmp_pipe_list->pre.push_back((*j));
      					pipe_vec.erase(j);
      				}else {
      					j++ ;
      				}
      			}
      			pipe_vec.push_back(tmp_pipe_list);

      		}else if( i == args.size()-1  ) {
      			tmp_vec.push_back(args[i]);
      			pipe_list* tmp_pipe_list = new pipe_list ;
				int pipe_int = 0 ;
				for ( auto k : tmp_vec ) {
      				tmp_pipe_list->line.push_back(k);
      			}
      			tmp_pipe_list->cmd = tmp_char ;
      			tmp_pipe_list->cnt = pipe_int ;

      			tmp_vec.clear();
      			

      			for ( auto j = pipe_vec.begin() ; j != pipe_vec.end(); ) {
      				(*j)->cnt = (*j)->cnt -1 ;
      				if ( (*j)->cnt == 0 ){
      					(*j)->next = tmp_pipe_list ;
      					tmp_pipe_list->pre.push_back((*j));
      					pipe_vec.erase(j);
      				}else {
      					j++ ;
      				}
      			}
      			pipe_vec.push_back(tmp_pipe_list);

      		}else {
      			tmp_vec.push_back(args[i]);	
      		}	
      		
      	}
// cut command
    	
      	for ( auto i = pipe_vec.begin() ; i != pipe_vec.end() ;){
      		if ( (*i)->cnt == 0 ) {
	      		cout << (*i)->cmd << " " << (*i)->cnt << endl ;
	      		for ( auto j : (*i)->line ) {
	      			cout << j << " " ;
	      		}
	      		cout << endl ;
	      		cout << "pre.size() : "<< (*i)->pre.size() << endl;
	      		//cout << (*i)->pre[0]->cmd << endl ; 
	      		for ( auto k : (*i)->pre ) {
	      			cout << k->cmd << "  " << k->pre.size() << endl ;
	      		}
	      		pipe_vec.erase(i);
	      	}else {
	      		i++;
	      	}
      	}

      	// int ii = 0 ;
      	// char* cmd_vec[256];
      	// for ( ii = 0 ; ii < args.size() ; ii++ ){
      	// 	cmd_vec[ii] = args[ii] ;
      	// }
      	// *(cmd_vec+ii) = NULL ;
     	
// run command 
/*
		if ( !strcmp("exit",args[0]) ) {
			break;
		}else if ( !strcmp("printenv",args[0]) ) {
	    	cout << getenv() << endl ;
	    }else if ( !strcmp("setenv",args[0]) ) {
	    	setenv(tt,tt,1);
	    }else {
	    	if ( fork() == 0 ) {
	    		execvp( arr0 , cmd_vec );
	    	}else {
	    		wait(status);
	    	}
	    }
*/
	}

	return 0;
}