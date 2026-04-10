#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "../WAL/WAL.h"
#include "../TheradPool/ThreadPool.h"
#include "../parser/parser.h"
#include "../store/DataStore.h"

#pragma comment (lib, "ws2_32.lib")
//TcpServer.h
class TcpServer {

public:
    TcpServer();
	int initWinsock(); //initialise winsock
	int makeListeningSocket(); //create listining socket
	int bindIPandPORT();  //bind ip and port
	int startListining(); //start listining
	int handleClient(SOCKET clientSocket);
	int startServer();
private:
	WSADATA ws;
	SOCKET m_listining;
	sockaddr_in m_serverAddr, m_client;
	int m_port = 8080;
    Parser m_parser;
	myDB::DataStore m_datastore;
	WAL m_wal;
	ThreadPool m_threadPool;


};