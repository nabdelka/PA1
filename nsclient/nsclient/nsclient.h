#pragma once

#define MAX_DOMAIN_NAME_LEN 255
#define DNS_PORT 53

int main_program(char *dns_ip_address);
bool is_quit(char *str);
bool is_legal(char *str);