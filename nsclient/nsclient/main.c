#include <stdio.h>
#include <stdbool.h>

#include "nsclient_header.h"



int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Please provid the DNS server IP.\n");
		//return -1;
	}
	//if (ip_checker(argv[1]) == -1) return -1;

	//return main_program(argv[1]); 
	return main_program("10.0.0.138"); 
	
}




