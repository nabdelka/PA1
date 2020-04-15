#pragma once

#define MAX_DOMAIN_NAME_LEN 257
#define MAX_IP_ADDRESS_LEN 16
#define MAX_LABEL_LEN 72
#define DNS_PORT 53
#define MAX_MSG_LEN 512

#define LINES_IN_HEADER 6

#define MAX_WAIT_TIME_SEC 2
#define MAX_WAIT_TIME_USEC 0

enum header {
	ID,
	FLAGS,
	QCOUNT,
	ANCOUNT,
	NSCOUNT,
	ARCOUNT
};


// Functions declarations
int main_program(char *dns_ip_address);
bool is_quit(char *str);
bool is_legal(char *str);
bool is_letter(char digit);
bool is_num(char num);
int header_checker(unsigned char *header);
void convert_hostname(char *source, char dest[255]);
int send_msg_and_rcv_rspns(char * send_buf, int msg_len, char rcv_buf[MAX_MSG_LEN]);
