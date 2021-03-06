#include "stdafx.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Initialze winsock
	// We need to call WSAStartup , therefore we define the following two parameters
	WSADATA wsData; // Pointer to the WSADATA structure required by the WSAStartup function
	WORD ver = MAKEWORD(2, 2); // Highest version of Windows Sockets specification that the caller can use.

	int wsOk = WSAStartup(ver, &wsData); // Calling WSAStartup function, if successful the return value is 0
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock - Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // Creates a socket called listening of the type SOCKET with the function socket. socket needs an address family, the socket type which is SOCK_STREAM since we want a TCP socket
	if (listening == INVALID_SOCKET) // INVALID_SOCKET is a debug value. Windows Sockets have no restrictions other than this value
	{
		cerr << "Can't create a socket - Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint; // The sockaddr_in structure is used by Windows Sockets to specify a local endpoint address
	hint.sin_family = AF_INET; // Defines the sin_family parameter, this must be AF_INET
	hint.sin_port = htons(54000); // We need to specify the port being used. To do this we need the htons function which stands for Host-TO-Network-Short. We need this because networking used big endian and pc's use little endian
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // This is where we specify the IP address that we're utilizing. Basically, we're using any IP which is available 

	bind(listening, (sockaddr*)&hint, sizeof(hint)); // Bind the name of the socket "listening" to a pointer to the socket address with the size of the hint structure

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN); // This marks the sockets to be for listening. It doesn't accept any incoming connections. SOMAXCONN is the backlog. Since we chose it to be SOMAXCONN, the underlying service provider responsible for the socket will define it to be the maximum reasonable value

	// Wait for a connection
	sockaddr_in client; // Creating another sockaddr_in structure called client
	int clientSize = sizeof(client); // We specify the size of this client for later use

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize); // Since the connecting client will return a random number as its port, we need to accept this. We could do the same error handling here as with the previous socket

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // Fills host with zeroes to avoid any undesired effects of optimizing compilers
	ZeroMemory(service, NI_MAXSERV); // Fills service with zeroes for same reason

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // Aquires the name of the connected client from the Domain Name System (DNS) if possible. This returns 0 if successful
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST); // If we can't get the name of the client we want the IP address instead. Here we need the address family, pointer to the address of the client, name of our string buffer, size of string buffer
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[4096]; // Creates variable buf with a given size of 4096 bytes. You should be redefining this until you can read "no bytes"

	while (true)
	{
		ZeroMemory(buf, 4096); // Fills buf with zeroes for same reason

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0); // Defines bytesReceived as the received message from the socket
		if (bytesReceived == SOCKET_ERROR) // SOCKET_ERROR is equal to -1, which means there was an error in the transmition from client
		{
			cerr << "Error in recv() - Quitting" << endl;
			break;
		}

		if (bytesReceived == 0) // If this is true then the client has disconnect from the communication
		{
			cout << "Client disconnected " << endl;
			break;
		}

		//cout << string(buf, 0, bytesReceived) << endl;

		// Echo a message back to client
		if (*buf == '2') { // Checking value at pointer's location
			cout << "Server received " << *buf << endl;
			*buf = 'a';
			cout << "Server has sent " << *buf << " back" << endl;
		}

		if (*buf == '3') {
			cout << "Server received " << *buf << endl;
			*buf = 'b';
			cout << "Server has sent " << *buf << " back" << endl;
		}
			//cout << "buffer is " << buf << endl;
			send(clientSocket, buf, bytesReceived + 1, 0); // Sends data on the connected socket

	}

	// Close the socket
	closesocket(clientSocket); // Closes the existing socket

	// Cleanup winsock
	WSACleanup(); // Terminates use of Winsock 2 DLL
}
