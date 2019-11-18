//#include "shell.h"
#include "shmparser.h"
#include <functional>

using namespace std;


function<void(int)> callback_wrapper1;
void callback_function1(int value) {
	callback_wrapper1(value);
}

void start_shell() {
	
	//shell sh;
	

	

	shmparser shm;
	shm.client_login();
	//shm.who(1);

	callback_wrapper1 = bind1st(mem_fun(&shmparser::userpipehandler),&shm);
	struct sigaction sigHandler2;
	sigHandler2.sa_handler = callback_function1;
	sigHandler2.sa_flags = SA_NODEFER;
	sigaction(SIGUSR2,&sigHandler2,NULL);

	while(1) {
		
		
		
		char command[15005];
		shm.sh.reset_flag();
		if ( shm.sh.input_command(0,command) <= 0 ) {
			//cerr << "wowo\n";
			command[0] = '\0';
			continue;
		}
		//cerr << command << endl ;

		shm.load_pid();
		shm.load_user();
		//cerr << "user" << endl;
		shm.load_id();
		//cerr << "id" << endl;
		shm.load_pipe();
		//cerr << "pipe" << endl;

		shm.sh.command_strtok(command);
		shm.sh.parser(shm.get_id());

		shm.update_pipe();
		vector<boardcastmsg> allusermsg = shm.sh.getusermsg();
		shm.boardcastusermsg(allusermsg);
		shm.sh.run_command();
		shm.buildinrun();
		
		
		if ( shm.sh.exit() ){
			shm.logout();
			break;
		}
		shm.sh.clearusermsg();
		//shm.update_mytouservec();
		shm.update_user();
		command[0] = '\0';
		write(1,"% ",2);
		////cerr << "user" << endl;
		//shm.update_id();
		////cerr << "id" << endl;
		//shm.update_pipe();
		////cerr << "pipe" << endl;
		
	}


}