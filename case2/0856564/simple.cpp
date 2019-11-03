#include <iostream>
#include "user_man.h"

using namespace std;

int main () {
	//shell one;
	user_man user_man;
	user user;
	user_man.user_login(user);
	//user.user_goshell();
	user_man.user_run(0);
	user_man.who(0);
	
}