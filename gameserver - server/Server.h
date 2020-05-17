#pragma once
#include <iostream> 
#include <map> 
#include <WS2tcpip.h> 
#include <string>

using namespace std;

#define MAX_BUFFER 1024
#define SERVER_PORT 9000
#define MAX_USER 4

#define PACKET_CS_GAME_START 300
#define PACKET_SC_GAME_START 301

#define PACKET_SC_LOGIN 100
#define PACKET_SC_LOCATION 101
#define PACKET_SC_JUMP 102
#define PACKET_SC_PLAYERS 103

#define PACKET_CS_LOGIN 200
#define PACKET_CS_LOCATION 201
#define PACKET_CS_JUMP 202
#define PACKET_CS_PLAYERS 203

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

typedef struct Info_Player {
	int Host = -1;
	bool IsUsed[MAX_USER] = { false };
	Location Loc[MAX_USER];
	bool IsJump[MAX_USER] = { false };
}Player;

typedef struct Test_Packet {
	int packet_type;
	int i;
}R_Test;


typedef struct Recv_Packet_Location {
	int packet_type;
	Location clientLoc;
}R_Loc;

typedef struct Recv_Packet_Jump {
	int packet_type;
	int i;
}R_Jump;

typedef struct Recv_Packet_Login {
	int packet_type = PACKET_CS_LOGIN;
}R_Login;

typedef struct Send_Packet_Login {
	int packet_type = PACKET_SC_LOGIN;
	int clientId;
	bool Player[MAX_USER] = { false };
	int Host;
	//Location Loc[MAX_USER];
}S_Login;

typedef struct Send_Packet_GAME_START {
	int packet_type = PACKET_SC_GAME_START;
	bool Started;
}S_Start;

typedef struct Recv_Packet_GAME_START {
	int packet_type = PACKET_CS_GAME_START;
}R_Start;

typedef struct Send_Packet_Players {
	int packet_type = PACKET_SC_PLAYERS;
	bool IsUsed[MAX_USER];
	Location Loc[MAX_USER];
	bool IsJump[MAX_USER];
}S_Players;

typedef struct Recv_Packet_Players {
	int packet_type = PACKET_CS_PLAYERS;
	Location Loc;
	bool IsJump;
}R_Players;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

int GetId();
void Recv_Packet(int clientId, char* buf);
void Send_Packet(char* buf);