#pragma once
#include <iostream> 
#include <map> 
#include <WS2tcpip.h> 
#include <string>

using namespace std;

#define MAX_BUFFER 1024
#define SERVER_PORT 9000
#define MAX_USER 4

#define PACKET_SC_LOGIN 100
#define PACKET_SC_LOCATION 101
#define PACKET_SC_JUMP 102

#define PACKET_CS_LOCATION 201
#define PACKET_CS_JUMP 202

typedef struct LOCATION {
	float x;
	float y;
	float z;
}Location;

struct OVER_EX {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
};

typedef struct SOCKETINFORM {
	OVER_EX over;
	SOCKET socket;
	int clientId;
	bool isUsed = false;
}SockInf;

typedef struct RECVOBJECT {
	int clientId;
	bool isUsed[MAX_USER] = { false };
	//bool keyBuffer[MAX_BUFFER];
	Location clientLoc;
}R_Obj;

typedef struct SENDOBJECT {
	int clientId;
	bool isUsed[MAX_USER] = { false };
	Location clientLoc[MAX_USER];
}S_Obj;

typedef struct PLAYER {
	int clientid;
	Location clientLoc = { 0 ,0, 0 };
}Player_Obj;

typedef struct Test_Packet {
	int packet_type;
	int i;
}R_Test;

typedef struct Recv_Packet_Location {
	int packet_type;
	Location clientLoc;
}R_Loc;

typedef struct Send_Packet_Login {
	int packet_type = PACKET_SC_LOGIN;
	int clientId;
	bool Player[4];
}S_Login;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

int GetId();
void Recv_Packet(int clientId, char* buf);
void Send_Packet(void* packet);