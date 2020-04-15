#include <stdio.h>


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Please provid the DNS server IP.\n");
		//return -1; TODO enable
	}
	// TODO - do we need to check input syntax?
	//check ip
	ip_checker(argv);

	//return main_program(argv[1]); TODO enable
	return main_program("10.0.0.138"); // TODO remove
	//return main_program("8.8.8.8"); // TODO remove
	
}
void ip_checker(char *header) {
	int i, num=0 ,points = 3, nums = 4;
	while (header[i] != '\0') {
		if (header[i] != '.') {
			num = num * 10 + (header[i] - '0');
		}
		else {
			if (num < 0 || num>255) {
				printf("unvalid ip num!");
				return;
			}
		}
		points = points - 1;
		nums = nums - 1;
		if (points < 0 || nums<0) {
			printf("unvalid ip num!");
			return;
		}
	}
	return;
}


