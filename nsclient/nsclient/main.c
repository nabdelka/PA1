#include <stdio.h>


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("One argument expected.\n");
		//return -1;
	}
	// TODO - do we need to check input syntax?
	

	//return main_program(argv[1]); TODO
	return main_program("10.0.0.138");
	//return main_program("127.0.0.1");

	
}

