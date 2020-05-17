#include "Server.h"
#include <stdio.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib") 

int id = 0;

SockInf g_clients[MAX_USER];
R_Obj recvInform;
S_Obj sendInform;
Player Player_Info;

bool IsStarted = false;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	int clientId = reinterpret_cast<int>(overlapped->hEvent);

	// Ŭ���̾�Ʈ�� closesocket�� ���� ���
	if (dataBytes == 0) {
		closesocket(g_clients[clientId].socket);
		cout << clientId + 1 << "�� �÷��̾� ���� (recv_callback)" << endl;
		g_clients[clientId].isUsed = false;
		recvInform.isUsed[clientId] = false;
		Player_Info.IsUsed[clientId] = false;
		return;
	}

	Recv_Packet(clientId, g_clients[clientId].over.dataBuffer.buf);

	// &dataBytes -> �̰� ���Ϲ޴°ǵ� ���°� �� ���ٰ� �����ϼ���.

	/*for (int i = 0; i < MAX_USER; ++i) {
		if (Player_Info.IsUsed[i]) {*/
		/*	WSASend(g_clients[clientId].socket, &(g_clients[clientId].over.dataBuffer), 1, NULL, 0,
				&(g_clients[clientId].over.overlapped), send_callback);*/
				/*	}
				}*/
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD flags = 0;

	int clientId = reinterpret_cast<int>(overlapped->hEvent);

	// Ŭ���̾�Ʈ�� closesocket�� ���� ���
	if (dataBytes == 0) {
		closesocket(g_clients[clientId].socket);
		cout << clientId + 1 << "�� �÷��̾� ���� (send_callback)" << endl;
		g_clients[clientId].isUsed = false;
		sendInform.isUsed[clientId] = false;
		Player_Info.IsUsed[clientId] = false;
		return;
	}

	g_clients[clientId].over.dataBuffer.len = MAX_BUFFER;
	g_clients[clientId].over.dataBuffer.buf = reinterpret_cast<char*>(&recvInform);

	memset(&(g_clients[clientId].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
	g_clients[clientId].over.overlapped.hEvent = (HANDLE)clientId;


	WSARecv(g_clients[clientId].socket, &g_clients[clientId].over.dataBuffer, 1, 0, &flags,
		&(g_clients[clientId].over.overlapped), recv_callback);

}

int GetId()
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (g_clients[i].isUsed == false) {
			g_clients[i].isUsed = true;
			return i;
		}
	}
	return -1;
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));

	listen(listenSocket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSocket != INVALID_SOCKET) {
			id = GetId();
			cout << id + 1 << "�� Ŭ���̾�Ʈ ����" << endl;
		}


		if (id < 0) {
			closesocket(clientSocket);
			continue;
		}

		memset(&g_clients[id], 0x00, sizeof(SockInf));
		g_clients[id].socket = clientSocket;
		g_clients[id].over.dataBuffer.len = sizeof(recvInform);
		g_clients[id].over.dataBuffer.buf = reinterpret_cast<char*>(&recvInform);
		ZeroMemory(&(g_clients[id].over.overlapped), sizeof(WSAOVERLAPPED));
		g_clients[id].clientId = id;
		g_clients[id].isUsed = true;
		flags = 0;

		recvInform.clientId = id;
		for (int i = 0; i < MAX_USER; ++i) {
			if (true == g_clients[i].isUsed) {
				recvInform.isUsed[i] = g_clients[i].isUsed;
				Player_Info.IsUsed[i] = g_clients[i].isUsed;
			}
		}

		g_clients[id].over.overlapped.hEvent = (HANDLE)id;

		// 1�� ������ ����! �츮�� �ϳ� �� ���̴�. ���δ�� MAX���� �� ��.
		// Recv ó���� 'recv_callback' ���� �Ѵ�.

		WSARecv(g_clients[id].socket, &g_clients[id].over.dataBuffer, 1, 0, &flags,
			&(g_clients[id].over.overlapped), recv_callback);
	}

	closesocket(listenSocket);
	WSACleanup();
}

void Recv_Packet(int clientId, char* buf) {

	R_Test* Test = reinterpret_cast<R_Test*>(buf);

	if (Test->packet_type == PACKET_CS_LOGIN) {
		cout << clientId + 1 << "�� �÷��̾� Login! " << endl;
		Player_Info.IsUsed[clientId] = true;
		if (Player_Info.Host == -1)
			Player_Info.Host = clientId;

		S_Login packet;
		packet.clientId = clientId;
		for (int i = 0; i < MAX_USER; ++i) {
			packet.Player[i] = Player_Info.IsUsed[i];  // ����
			packet.Host = Player_Info.Host;
			cout << "�÷��̾� " << i + 1 << ": " << Player_Info.IsUsed[i] << endl;
		}
		cout << "���� : �÷��̾� " << Player_Info.Host << endl;

		for (int i = 0; i < MAX_USER; ++i) {
			if (Player_Info.IsUsed[i]) {
				g_clients[i].over.dataBuffer.len = sizeof(packet);
				memset(&(g_clients[i].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
				g_clients[i].over.overlapped.hEvent = (HANDLE)i;

				g_clients[i].over.dataBuffer.buf = reinterpret_cast<char*>(&packet);

				WSASend(g_clients[i].socket, &(g_clients[i].over.dataBuffer), 1, NULL, 0,	 // ����
					&(g_clients[i].over.overlapped), send_callback);
				cout << i + 1 << "�� �÷��̾�� �α������� ����!" << endl;
			}
		}
	}
	else {
		S_Players tmp;
		switch (Test->packet_type)
		{
		case PACKET_CS_GAME_START:
		{
			//R_Start* packet = reinterpret_cast<R_Start*>(buf);
			S_Start packet;
			IsStarted = true;
			packet.Started = IsStarted;
			cout << "ȣ��Ʈ�� ���� ������" << endl;

			for (int i = 0; i < MAX_USER; ++i) {
				if (Player_Info.IsUsed[i]) {
					g_clients[i].over.dataBuffer.len = sizeof(packet);
					memset(&(g_clients[i].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
					g_clients[i].over.overlapped.hEvent = (HANDLE)i;

					g_clients[i].over.dataBuffer.buf = reinterpret_cast<char*>(&packet);

					WSASend(g_clients[i].socket, &(g_clients[i].over.dataBuffer), 1, NULL, 0,	 // ����
						&(g_clients[i].over.overlapped), send_callback);
					cout << i + 1 << "�� �÷��̾�� ���ӽ��� ����!" << endl;
				}
			}
		}
		break;
		case PACKET_CS_LOCATION:
		{
			R_Loc* packet = reinterpret_cast<R_Loc*>(buf);
			cout << clientId + 1 << "�� �÷��̾� Location ( ";
			cout << "X : " << packet->clientLoc.x << ", Y : " << packet->clientLoc.y << ", Z : " << packet->clientLoc.z << ")" << endl;
			cout << "//////////////////////////////////////////////////////////////////" << endl;
			Player_Info.Loc[clientId] = packet->clientLoc;

			for (int i = 0; i < MAX_USER; ++i) {
				tmp.IsUsed[i] = Player_Info.IsUsed[i];
				tmp.Loc[i] = Player_Info.Loc[i];
				tmp.IsUsed[i] = Player_Info.IsJump[i];
			}

			g_clients[clientId].over.dataBuffer.len = sizeof(tmp);
			memset(&(g_clients[clientId].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
			g_clients[clientId].over.overlapped.hEvent = (HANDLE)clientId;

			g_clients[clientId].over.dataBuffer.buf = reinterpret_cast<char*>(&tmp);

			for (int i = 0; i < MAX_USER; ++i) {
				if (Player_Info.IsUsed[i]) {
					WSASend(g_clients[i].socket, &(g_clients[i].over.dataBuffer), 1, NULL, 0,
						&(g_clients[i].over.overlapped), send_callback);
				}
			}
		}
		break;
		case PACKET_CS_JUMP:
		{
			R_Jump* packet = reinterpret_cast<R_Jump*>(buf);
			cout << clientId + 1 << "�� �÷��̾� Jump!" << endl;
			cout << "//////////////////////////////////////////////////////////////////" << endl;
			Player_Info.IsJump[clientId] = true;

			for (int i = 0; i < MAX_USER; ++i) {
				tmp.IsUsed[i] = Player_Info.IsUsed[i];
				tmp.Loc[i] = Player_Info.Loc[i];
				tmp.IsUsed[i] = Player_Info.IsJump[i];
			}

			g_clients[clientId].over.dataBuffer.len = sizeof(tmp);
			memset(&(g_clients[clientId].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
			g_clients[clientId].over.overlapped.hEvent = (HANDLE)clientId;

			g_clients[clientId].over.dataBuffer.buf = reinterpret_cast<char*>(&tmp);

			for (int i = 0; i < MAX_USER; ++i) {
				if (Player_Info.IsUsed[i]) {
					WSASend(g_clients[i].socket, &(g_clients[i].over.dataBuffer), 1, NULL, 0,
						&(g_clients[i].over.overlapped), send_callback);
				}
			}
		}
		break;
		case PACKET_CS_PLAYERS:
		{
			R_Players* packet = reinterpret_cast<R_Players*>(buf);
			cout << clientId + 1 << "�� �÷��̾� ���� RECV !!!" << endl;
			cout << "//////////////////////////////////////////////////////////////////" << endl;

			Player_Info.Loc[clientId] = packet->Loc;
			Player_Info.IsJump[clientId] = packet->IsJump;

			S_Players s_packet;
			for (int i = 0; i < MAX_USER; ++i) {
				s_packet.IsUsed[i] = Player_Info.IsUsed[i];
				s_packet.Loc[i] = Player_Info.Loc[i];
				s_packet.IsJump[i] = Player_Info.IsJump[i];
			}

			g_clients[clientId].over.dataBuffer.len = sizeof(s_packet);
			memset(&(g_clients[clientId].over.overlapped), 0x00, sizeof(WSAOVERLAPPED));
			g_clients[clientId].over.overlapped.hEvent = (HANDLE)clientId;

			g_clients[clientId].over.dataBuffer.buf = reinterpret_cast<char*>(&s_packet);

			for (int i = 0; i < MAX_USER; ++i) {
				if (Player_Info.IsUsed[i]) {
					WSASend(g_clients[i].socket, &(g_clients[i].over.dataBuffer), 1, NULL, 0,
						&(g_clients[i].over.overlapped), send_callback);
				}
			}

		}
		break;
		}
		
	}
}

void Send_Packet(char* buf) {

}