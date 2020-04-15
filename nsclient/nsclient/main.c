#include <stdio.h>


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Please provid the DNS server IP.\n");
		//return -1; TODO enable
	}
	// TODO - do we need to check input syntax?
	

	//return main_program(argv[1]); TODO enable
	return main_program("10.0.0.138");
	//return main_program("8.8.8.8");
	//return main_program("127.0.0.1");

	
}

