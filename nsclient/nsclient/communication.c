#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "winsock2.h"
#include "nsclient_header.h"

#pragma comment(lib, "Ws2_32.lib")

 
// Global string - DNS IP from command line
extern char dns_ip_address_global[MAX_IP_ADDRESS_LEN];


int send_msg_and_rcv_rspns(char * send_buf, int msg_len, char rcv_buf[MAX_MSG_LEN]) {

	// Create Socket
	SOCKET SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (SendSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	struct sockaddr_in     servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information 
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DNS_PORT);
	servaddr.sin_addr.s_addr = inet_addr(dns_ip_address_global);

	// Send all message to the receiver
	int already_sent = 0;
	int iResult = -1;

continue_sending:

	iResult = sendto(SendSocket, send_buf + already_sent, msg_len - already_sent, 0, (SOCKADDR *)& servaddr, sizeof(servaddr));
	if (iResult >= 0) {
		already_sent += iResult;
		if (already_sent != msg_len) goto continue_sending;
	}
	else { // iResult == SOCKET_ERROR
		perror("sendto failed");
		closesocket(SendSocket);
		WSACleanup();
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = MAX_WAIT_TIME_SEC;
	tv.tv_usec = MAX_WAIT_TIME_USEC;
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(SendSocket, &read_fds);

	iResult = select(SendSocket + 1, &read_fds, NULL, NULL, &tv);
	if (iResult == 0) {
		printf("TIMEOUT\n");
		return -1;
	}
	else if (iResult < 0) {
		perror("select failed");
		return -1;
	}

	// Receive response
	iResult = recv(SendSocket, rcv_buf, MAX_MSG_LEN, 0); 
	if (iResult <= 0)
	{
		perror("recv failed");
		return -1;
	}


	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) {
		perror("close socket failed");
		return -1;
	}

	return iResult;
}