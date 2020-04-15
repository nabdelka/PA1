#include <stdio.h>


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Please provid the DNS server IP.\n");
		//return -1; TODO enable
	}
	// TODO - check input syntax?
	// Check argv[1]
	

	//return main_program(argv[1]); TODO enable
	return main_program("10.0.0.138"); // TODO remove
	//return main_program("8.8.8.8"); // TODO remove
	
}

