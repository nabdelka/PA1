
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
//  https://moodle.tau.ac.il/mod/forum/discuss.php?d=72022

void dnsQuery(char *host_name, char *dns_server_ip) { // TODO change return type to struct

	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("Error while initializing windows networking\n");
		return -1;
	}

	// Create Socket
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s = INVALID_SOCKET) {
		printf("Error while creating socket\n");
		return -1;
	}

	struct sockaddr_in     servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information 
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DNS_PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY; // TODO server ip address

}

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
			}


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
	return true;
}

bool is_quit(char *str) {
	// TODO check if containing single quit word
	if( strcmp(str,"quit") == 0) return true;
	else                         return false;
}