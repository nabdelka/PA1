
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "winsock2.h"
#include "nsclient_header.h"

#pragma comment(lib, "Ws2_32.lib")


// Global string - DNS IP
char dns_ip_address_global[MAX_IP_ADDRESS_LEN];
// Global ID counter
short id_counter = 1;
int ip_counter = 0;


int read_question(char *buffer, int start_idx){

	bool check_len = TRUE;
	int cur_len = -1;
	int j;

	for (j = start_idx; j < MAX_MSG_LEN; j++) {
		if (buffer[j] == 0) break;
		if (check_len) {
			if ((buffer[j] >> 6) & 0x3) {
				//comppressed
				j += 1;
				break;
			}
			check_len = FALSE;
			cur_len = buffer[j];
			continue;
		}
		else {
			cur_len--;
			if (cur_len == 0) check_len = TRUE;
		}

	}
	return j + 5; // Qtype and Qclass

}

int read_answer(char *buffer, int start_idx, unsigned char ip_buffer[4]) {

	bool check_len = TRUE;
	int cur_len = -1;
	int j;

	for (j = start_idx; j < MAX_MSG_LEN; j++) {
		if (buffer[j] == 0) break;
		if (check_len) {
			if ((buffer[j] >> 6) & 0x3) {
				//comppressed
				j += 1;
				break;
			}
			check_len = FALSE;
			cur_len = buffer[j];
			continue;
		}
		else {
			cur_len--;
			if (cur_len == 0) check_len = TRUE;
		}

	}
	j = j + 1; // Qtype and Qclass
	unsigned short type = ((buffer[j] << 8) | buffer[j + 1]);
	j += 8; // type and class and ttl
	unsigned short rdlength = ((buffer[j] << 8) | buffer[j + 1]);
	j += 2; // rdlength

	ip_buffer[0] = buffer[j];
	ip_buffer[1] = buffer[j + 1];
	ip_buffer[2] = buffer[j + 2];
	ip_buffer[3] = buffer[j + 3];
	
	if ((type == 1)&(rdlength == 4)) {

		ip_buffer[4] = 0;
	}
	else {
		// not IP
		ip_buffer[4] = 1;
	}

	return j + rdlength;

}

struct hostent *dnsQuery(char *host_name) {

	char hostname_labels[257];
	convert_hostname(host_name, hostname_labels);
	ip_counter = 0;
	short header_array[LINES_IN_HEADER];
	char send_buff[MAX_MSG_LEN];
	char unsigned recv_buf[MAX_MSG_LEN];
	
	// Header fill
	header_array[ID] = id_counter;
	header_array[FLAGS] = 0x0100;
	header_array[QCOUNT] = 1;
	header_array[ANCOUNT] = 0;
	header_array[NSCOUNT] = 0;
	header_array[ARCOUNT] = 0;
	// Copy header to send buffer
	for (int j = 0; j < LINES_IN_HEADER; j++) {
		send_buff[2 * j] = header_array[j] >> 8;
		send_buff[2 * j + 1] = header_array[j] & 0xFF;
	}
	// Copy host_name to send buffer
	int i = 0;
	while (hostname_labels[i] != 0) {
		send_buff[LINES_IN_HEADER * 2 + i] = hostname_labels[i];
		i++;
	}
	send_buff[LINES_IN_HEADER * 2 + i] = 0;
	// QTYPE = 0x01
	send_buff[LINES_IN_HEADER * 2 + i + 1] = 0;
	send_buff[LINES_IN_HEADER * 2 + i + 2] = 1;
	// QCLASS = 0x01
	send_buff[LINES_IN_HEADER * 2 + i + 3] = 0;
	send_buff[LINES_IN_HEADER * 2 + i + 4] = 1;


	// Send msg ( length =  LINES_IN_HEADER * 2 + i + 5) and got rcved msg
	int check = send_msg_and_rcv_rspns(send_buff, LINES_IN_HEADER * 2 + i + 5, recv_buf);
	if (check < 0) return NULL;

	check = header_checker(recv_buf);
	if (check < 0) return NULL;

	// Allocate memory for hostent struct and init
	struct hostent *hostent_ptr = get_init_hostent_ptr();
	if (hostent_ptr == NULL)	return NULL;

	int cur_idx = read_question(recv_buf , LINES_IN_HEADER * 2);
	char ip[5];
	for (int k = 0; k < check; k++) {
		cur_idx = read_answer(recv_buf, cur_idx, ip);
		if (ip[4] == 0) {
			if (add_ip(hostent_ptr, ip) != 0) return NULL;
			ip_counter++;
		}
	}
	   	  
	id_counter++;
	return hostent_ptr;
}



void convert_hostname(char *source, char dest[255]) {
	int i = 0, len = 0, startlabel = 0;
	char hostname[257];
	while (source[i] != '\0') {
		if (source[i] != '.') {
			dest[i + 1] = source[i];
			len = len + 1;
			i = i + 1;
		}
		else {
			i = i + 1;
			dest[startlabel] = len;
			len = 0;
			startlabel = i;
		}
	}
	if (source[i] == '\0') {
		dest[startlabel] = len;
		dest[i + 1] = 0;
	}

}

struct hostent *get_init_hostent_ptr() {
	struct hostent *ptr = (struct hostent *)malloc(sizeof(struct hostent));
	char **addr_list = (char **)malloc(sizeof(char*));
	if ((ptr == NULL)|(addr_list == NULL)) {
		perror("Memory Error");
		return NULL;
	}
	addr_list[0] = NULL;
	ptr->h_name = NULL;
	ptr->h_aliases = NULL;
	ptr->h_addrtype = AF_INET;
	ptr->h_length = 4;
	ptr->h_addr_list = addr_list;
	return ptr;

}

int add_ip(struct hostent *ptr, char ip[5]) {
	ptr->h_addr_list = realloc(ptr->h_addr_list, (ip_counter + 2) * sizeof(char*));
	char *new_ip = (char *)malloc(4 * sizeof(char));
	if ((ptr->h_addr_list == NULL) | (new_ip == NULL)) {
		perror("Memory Error");
		return -1;
	}
	new_ip[0] = ip[0];
	new_ip[1] = ip[1];
	new_ip[2] = ip[2];
	new_ip[3] = ip[3];

	ptr->h_addr_list[ip_counter] = new_ip;
	ptr->h_addr_list[ip_counter + 1] = NULL;			
	return 0;
}

void free_hostent_memory(struct hostent *ptr) {
	int i = 0;
	while (ptr->h_addr_list[i] != NULL) {
		free(ptr->h_addr_list[i]);
		i++;
	}
	free(ptr->h_addr_list);
	free(ptr);

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

	// Install ip address from command line to global buffer
	errno_t err_code = strcpy_s(dns_ip_address_global, MAX_IP_ADDRESS_LEN, dns_ip_address);
	if (err_code != 0) {
		printf("Error while Copying IP address. please check lenght\n");
		return -1;
	}

	// program loop
	while (1) {

		printf("nsclient> ");
		gets(domain_name_str);

		if (is_quit(domain_name_str)) break;

		// check input syntax
		if (is_legal(domain_name_str)) {
			//	if good -> call dnsQuery
			remoteHost = dnsQuery(domain_name_str);
			if (remoteHost != NULL) {
				if (remoteHost->h_addrtype == AF_INET) {
					if( remoteHost->h_addr_list[0] != NULL ) {
						printf("%d.%d.%d.%d\n", (unsigned char)remoteHost->h_addr_list[0][0], (unsigned char)remoteHost->h_addr_list[0][1], (unsigned char)remoteHost->h_addr_list[0][2], (unsigned char)remoteHost->h_addr_list[0][3]);
					}
				}
				free_hostent_memory(remoteHost);
			}
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
	if (!(is_letter(str[0]))) {
		return false;
	}
	while (str[i] != '\0') {
		if (str[i] == '.' && str[i + 1] != '\0') {
			if (!(is_letter(str[i + 1]))) {
				return false;
			}
			else if (!(is_letter(str[i - 1])) && !(is_num(str[i - 1]))) {
				return false;
			}
			i = i + 2;
			continue;
		}
		else if (str[i] == '.' && str[i + 1] == '/0') {
			return false;
		}
		else if (!(is_letter(str[i])) && !(is_num(str[i])) && !(str[i] == '_')) {
			return false;
		}
		i = i + 1;
	}
	if ((str[i] == '\0') && str[i - 1] == '_') {
		return false;
	}
	return true;

}

bool is_num(char num) {
	if (num >= '0' && num <= '9') return true;
	else                         return false;
}

bool is_letter(char digit) {
	if (((digit >= 'A') && (digit <= 'Z')) || ((digit >= 'a') && (digit <= 'z'))) return true;
	else                         return false;
	
}

bool is_quit(char *str) {
	if( strcmp(str,"quit") == 0) return true;
	else                         return false;
}

int header_checker(unsigned char *header) {
	int i = 0;
	unsigned char id, QR, Opcode, AA, TC, RD, RA, Z, RCODE;
	char ANCOUNT, QDCOUNT, NSCOUNT, ARCOUNT;

	// Checking ID in header
	id = (header[0] << 8) | header[1];
	if (id != id_counter) {
		printf("TIMEOUT\n");
		return -1;
	}
	// Checking QR
	QR = header[2] & 0x80;
	QR = QR >> 7;
	if (QR != 1) {
		printf("Error: expected QR=1\n");
		return -1;
	}
	// Checking Opcode
	Opcode = header[2] & 0x78;
	Opcode = Opcode >> 3;
	if (Opcode != 0) {
		printf("Error: expected Opcode=0\n");
		return -1;
	}
	// No need to check
	AA = header[2] & 0x04;
	AA = AA >> 2;
	TC = header[2] & 0x02;
	TC = TC >> 1;
	RD = header[2] & 0x01;
	RA = header[3] & 0x80;
	RA = RA >> 7;
	Z = header[3] & 0x70;
	Z = Z >> 4;
	// Checking RCODE
	RCODE = header[3] & 0x0F;
	if (RCODE == 1) {
		printf("Unexpected format error!\n");
		return -1;
	}
	else if (RCODE == 2) {
		printf("ERROR: SERVER FAILURE\n");
		return -1;
	}
	else if (RCODE == 3) {
		printf("ERROR: NONEXISTENT\n");
		return -1;
	}
	else if (RCODE == 4) {
		printf("ERROR: NOT IMPLEMNTED\n");
		return -1;
	}
	else if (RCODE == 5) {
		printf("ERROR: REFUSED\n");
		return -1;
	}
	QDCOUNT = (header[4] << 8) | header[5];
	if (QDCOUNT != 1) {
		printf("Error: expected QDCOUNT=1\n");
		return -1;
	}
	ANCOUNT = (header[6] << 8) | header[7];

	// Not relevant
	NSCOUNT = (header[8] << 8) | header[9];
	ARCOUNT = (header[10] << 8) | header[11];
	return ANCOUNT;
}

int ip_checker(char *ip) {
	int i = 0, num = 0, points = 3, nums = 4;
	while (ip[i] != '\0') {
		if (ip[i] != '.') {
			num = num * 10 + (ip[i] - '0');
		}
		else {
			if (num < 0 || num>255) {
				printf("ERROR: BAD IP\n");
				return -1;
			}
		}
		points = points - 1;
		// this fails with input 10.0.0.38 due to nums more than 4 TODO
		nums = nums - 1;
		if (points < 0 || nums < 0) {
			printf("ERROR: BAD IP\n");
			return -1;
		}
	}
	return 0;
}


