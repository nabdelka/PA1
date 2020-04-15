
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "winsock2.h"
#include "nsclient.h"

#pragma comment(lib, "Ws2_32.lib")


// Noor's DNS server IP: 10.0.0.138
// UDP programming reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/ 
// HOSTENT structure: https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-hostent


// Global string - DNS IP
char dns_ip_address_global[MAX_IP_ADDRESS_LEN];
short id_counter = 0;

void dnsQuery(char *host_name) { // TODO change return type to struct

	//func to switch the hostname to the appropriate format
	// TODO move to function
	//short header_array[LINES_IN_HEADER];
	//for( int i=0; i< LINES_IN_HEADER; i++)	header_array[i] = 0;

	//header_array[QCOUNT] = 1;

	int i = 0, len = 0, startlabel = 0;
	char hostname[257];
	while (host_name[i] != '\0') {
		if (host_name[i] != '.') {
			hostname[i + 1] = host_name[i];
			len = len + 1;
			i = i + 1;
		}
		else {
			i = i + 1;
			hostname[startlabel] = len;
			len = 0;
			startlabel = i;
		}
	}
	if (host_name[i] == '\0') {
		hostname[startlabel] = len;
		hostname[i + 1] = 0;
	}
	// end of the func
	printf("Host name:\n");
	for (int j = 0; j < strlen(hostname) + 1; j++) printf("%d ", hostname[j]);
	printf("\n");

	
	short header_array[LINES_IN_HEADER]; //ID starts to count from 0
	header_array[FLAGS] = 0;
	header_array[ID] = 0;
	header_array[QCOUNT] = 1;
	header_array[ANCOUNT] = 0;
	header_array[NSCOUNT] = 0;
	header_array[ARCOUNT] = 0;

	char send_buff[512];
	for (int j = 0; j < LINES_IN_HEADER; j++) send_buff[2 * j] = header_array[j] >> 8;
	for (int j = 0; j < LINES_IN_HEADER; j++) send_buff[2 * j + 1] = header_array[j] && 0xFF;

	i = 0;
	while (hostname[i] != 0) {
		send_buff[LINES_IN_HEADER * 2 + i] = hostname[i];
		i++;
	}
	send_buff[LINES_IN_HEADER * 2 + i] = 0;
	send_buff[LINES_IN_HEADER * 2 + i + 1] = 0;
	send_buff[LINES_IN_HEADER * 2 + i + 2] = 1;
	send_buff[LINES_IN_HEADER * 2 + i + 3] = 0;
	send_buff[LINES_IN_HEADER * 2 + i + 4] = 1; //QCLASS For internet is IN




	for (int j = 0; j < LINES_IN_HEADER * 2 + i + 5; j++) {
		printf("%d ", send_buff[j]);
		if (j % 8 == 7) printf("\n");
	}
	printf("\n");



	char recv_buf[500];
	// TODO build a message
	int check = send_msg_and_rcv_rspns(send_buff, LINES_IN_HEADER * 2 + i + 5, recv_buf);



	return;

}

int send_msg_and_rcv_rspns(char * send_buf, int msg_len, char rcv_buf[500]);

int main_program(char *dns_ip_address) {
	char domain_name_str[MAX_DOMAIN_NAME_LEN + 1] = { 0 };
	struct hostent *remoteHost;

	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("Error while initializing windows networking\n");
		return -1;
	}

	// Install ip address from command line to global buffer
	errno_t err_code = strcpy_s(dns_ip_address_global, MAX_IP_ADDRESS_LEN, dns_ip_address);
	if (err_code != 0) {
		printf("Error while Copying IP address. please check lenght\n");
		return -1;
	}


	char buffer[132];
	// inf loop
	while (1) {

		printf("nsclient> ");
		gets(domain_name_str);

		if (is_quit(domain_name_str)) break;
		int i;
		// check input syntax
		if (is_legal(domain_name_str)) {
			//	if good -> call dnsQuery
			// TODO
			printf("GOOD NAME\n");
			dnsQuery(domain_name_str);
			/*
			remoteHost = gethostbyname(domain_name_str);
			char **pAlias;
			if (remoteHost == NULL) {
				DWORD dwError = WSAGetLastError();
				if (dwError != 0) {
					if (dwError == WSAHOST_NOT_FOUND) {
						printf("Host not found\n");
						return 1;
					}
					else if (dwError == WSANO_DATA) {
						printf("No data record found\n");
						return 1;
					}
					else {
						printf("Function failed with error: %ld\n", dwError);
						return 1;
					}
				}
			}
			else {
				printf("Function returned:\n");
				printf("\tOfficial name: %s\n", remoteHost->h_name);
				for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
					printf("\tAlternate name #%d: %s\n", ++i, *pAlias);
				}

				printf("\tAddress length: %d\n", remoteHost->h_length);

				i = 0;
				struct in_addr addr;
				if (remoteHost->h_addrtype == AF_INET)
				{
					while (remoteHost->h_addr_list[i] != 0) {
						addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
						printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
					}
				}
				else if (remoteHost->h_addrtype == AF_NETBIOS)
				{
					printf("NETBIOS address was returned\n");
				}
			}*/


		}
		else {
			// Error msg: illegal domain name
			printf("ERROR: BAD NAME\n");
		}


	}
	WSACleanup();

	return 0;


}


// Checking functions:

bool is_legal(char *str) {
	int i = 1;
	if (!(is_digit(str[0]))) {
		return false;
	}
	while (str[i] != '\0') {
		if (str[i] == '.' && str[i + 1] != '\0') {
			if (!(is_digit(str[i + 1]))) {
				return false;
			}
			else if (!(is_digit(str[i - 1])) && !(is_num(str[i - 1]))) {
				return false;
			}
			i = i + 2;
			continue;
		}
		else if (str[i] == '.' && str[i + 1] == '/0') {
			return false;
		}
		else if (!(is_digit(str[i])) && !(is_num(str[i])) && !(str[i] == '_')) {
			return false;
		}
		i = i + 1;
	}
	if ((str[i] == '\0') && str[i - 1] == '_') {
		return false;
	}
	return true;
	// TODO check in str if leggal return true/false
	// https://moodle.tau.ac.il/mod/forum/discuss.php?d=69030
}



bool is_num(char num) {
	if (num >= '0' && num <= '9') return true;
	else                         return false;
}


bool is_digit(char digit) {
	if (((digit >= 'A') && (digit <= 'Z')) || ((digit >= 'a') && (digit <= 'z'))) return true;
	else                         return false;
	
}

bool is_quit(char *str) {
	// TODO check if containing single quit word
	if( strcmp(str,"quit") == 0) return true;
	else                         return false;
}



