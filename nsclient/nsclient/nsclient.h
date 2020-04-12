#pragma once

#define MAX_DOMAIN_NAME_LEN 255
#define MAX_IP_ADDRESS_LEN 16
#define DNS_PORT 53

#define MAX_WAIT_TIME_SEC 2
#define MAX_WAIT_TIME_USEC 0

int main_program(char *dns_ip_address);
bool is_quit(char *str);
bool is_legal(char *str);