#include <stdbool.h>
#include <string.h>
#include "nsclient.h"


// TODO dnsQuery
//	-	got a string
//	- return hostnet

//  https://moodle.tau.ac.il/mod/forum/discuss.php?d=72022



int main_program(char *dns_ip_address) {

	char domain_name_str[MAX_DOMAIN_NAME_LEN + 1] = { 0 };

	// inf loop
	while (1) {

		printf("nsclient> ");
		gets(domain_name_str);

		if (is_quit(domain_name_str)) break;

		// check input syntax
		if (is_legal(domain_name_str)) {
			//	if good -> call dnsQuery
			// TODO

		}
		else {
			// Error msg: illegal domain name
			printf("ERROR: BAD NAME\n");
		}


	}

	return 0;


}


// Checking functions:

bool is_legal(char *str) {
	// TODO check in str if leggal return true/false
	// https://moodle.tau.ac.il/mod/forum/discuss.php?d=69030
	return false;
}

bool is_quit(char *str) {
	// TODO check if containing single quit word
	if( strcmp(str,"quit") == 0) return true;
	else                         return false;
}