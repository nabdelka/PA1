#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "winsock2.h"
#include "nsclient.h"

#pragma comment(lib, "Ws2_32.lib")

 
// Global string - DNS IP
extern char dns_ip_address_global[MAX_IP_ADDRESS_LEN];


int send_msg_and_rcv_rspns(char * send_buf, int msg_len, char rcv_buf[500]) {

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
	printf("Sending to IP: %s\n", dns_ip_address_global);

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
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		WSACleanup();
		return -1;
	}
	printf("Finished sending\n");
	SOCKET RecvSocket;

	struct timeval tv;
	tv.tv_sec = MAX_WAIT_TIME_SEC;
	tv.tv_usec = MAX_WAIT_TIME_USEC;
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(SendSocket, &read_fds);
	iResult = select(SendSocket + 1, &read_fds, NULL, NULL, &tv);
	if (iResult == 0) {
		printf("Select TIMEOUT\n");
		return 0;
	}
	else if (iResult < 0) {
		printf("Select FAILED\n");
		return -1;
	}

	
	// Receive until the peer closes the connection
	//rcv_again:
	iResult = recv(SendSocket, rcv_buf, 500, 0); // TODO parameter
	if (iResult > 0)
		printf("Bytes received: %d\n", iResult);
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());

	rcv_buf[iResult] = '\0';
	printf("Server : %s\n", rcv_buf);
	
	for (int j = 0; j < iResult; j++) printf("%d ", rcv_buf[j]);
	printf("\n");
	//goto rcv_again;
	//---------------------------------------------
	// When the application is finished sending, close the socket.
	printf("Closing socket.\n");
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) {
		printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	//---------------------------------------------
	return 0;

}