//Create a socket
//Bind it to a port
//Listen for incoming connections
//Accept a client connection
//Read data from the client
//Pass it to the parser and datastore
//Send a response back
//Handle multiple clients simultaneously — using your thread pool
//TcpServer.cpp
#include <iostream>
#include "TcpServer.h"
#include "../helpers/helper.h"
#define SEND_TO_CLIENT(response) send(clientSocket, response.c_str(), response.size(), 0);

int TcpServer::initWinsock()
{
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		LOG_ERROR("startup failed " << WSAGetLastError());
		return EXIT_FAILURE;
	}

	LOG_MSG("server started")
		return 0;
}

TcpServer::TcpServer()
	: m_listining(INVALID_SOCKET), m_serverAddr(), m_client(), m_port(8080), m_parser(), m_datastore(), m_wal(m_parser, m_datastore), m_threadPool(4)
{
}

int TcpServer::makeListeningSocket()
{
	m_listining = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listining < 0) {
		LOG_ERROR("socket creation failed " << WSAGetLastError());

		return EXIT_FAILURE;
	}
	LOG_MSG("socket created")
		return 0;
}

int TcpServer::bindIPandPORT()
{
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(m_port);
	m_serverAddr.sin_addr.s_addr = INADDR_ANY;
	int r = bind(m_listining, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
	if (r < 0) {
		LOG_ERROR("binding failed " << WSAGetLastError());
		closesocket(m_listining);
		WSACleanup();
		return EXIT_FAILURE;
	}
	LOG_MSG("IP AND PORT: " << m_port << " binded to server")
		return 0;
}

int TcpServer::startListining()
{
	int r = listen(m_listining, SOMAXCONN);
	if (r < 0) {
		LOG_ERROR("listen failed: " << WSAGetLastError());
		closesocket(m_listining);
		WSACleanup();
		return EXIT_FAILURE;
	}
	LOG("server is listining on " << m_port)
		return 0;

}



int TcpServer::handleClient(SOCKET clientSocket)
{
	while (true) {
		char recvBuff[512];
		int recvBufflen = 512;
		int r = recv(clientSocket, recvBuff, recvBufflen, 0);
		if (r <= 0)
		{
			if (r == 0) {
				LOG_MSG("client disconnected")
					std::string response = "you have disconnected\n";
				send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);


			}
			if (r < 0) {
				LOG_ERROR("process failed : " << WSAGetLastError());
				closesocket(clientSocket);
				return EXIT_FAILURE;
			}
		}
		std::string receivedData(recvBuff, r);
		// trim trailing \r and \n
		while (!receivedData.empty() && (receivedData.back() == '\r' || receivedData.back() == '\n'))
			receivedData.pop_back();

		if (receivedData.empty()) continue; // skip empty input, don't crash
		std::optional<parsedInformation> result;

		result = m_parser.parser(receivedData);



		if (result.has_value()) {
			auto r = result.value();
			if (r.cmd.has_value()) {
				switch (r.cmd.value()) {
					//1. SET command +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case commands::SET: {
					myDB::DataStore::DBVal val = r.value.value();
					std::string str;
					m_datastore.writeData(r.key, val);
					std::string valStr;
					if (std::holds_alternative<int>(val))
						valStr = std::to_string(std::get<int>(val));
					else if (std::holds_alternative<float>(val))
						valStr = std::to_string(std::get<float>(val));
					else
						valStr = std::get<std::string>(val);

					m_wal.writeLog("set " + r.key + " " + valStr);
					std::string response;
					response = r.key + " created and assigned to " + valStr + '\n';
					send(clientSocket, response.c_str(), response.size(), 0);

					break;
				}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case commands::GET: {
					std::string response;
					auto opt = m_datastore.readData(r.key);
					if (!opt.has_value()) {
						if (opt == std::nullopt) {
							response = r.key + " doesn't exist";

						}
						else {
							response = "GET DIDN'T RETURN ANYTHING";
						}
						send(clientSocket, response.c_str(), response.size(), 0);
						break;
					}

					auto val = opt.value();

					if (std::holds_alternative<std::string>(val)) {
						response = "string: " + std::get<std::string>(val) + "\n";

					}
					else if (std::holds_alternative<float>(val)) {
						float fv = std::get<float>(val);
						response = "float: " + std::to_string(fv) + "\n";

					}
					else if (std::holds_alternative<int>(val)) {
						int iv = std::get<int>(val);
						response = "int: " + std::to_string(iv) + "\n";
					}
					else {
						response = "FAILED TO GET DATA TYPE";
					}
					send(clientSocket, response.c_str(), response.size(), 0);

					break;
				}
								  //3. delete command +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case commands::DEL: {
					std::string response;
					std::string logstr = "delete " + r.key;
					m_datastore.deleteData(r.key);
					m_wal.writeLog(logstr);
					response = "deleted" + r.key + '\n';
					SEND_TO_CLIENT(response)
					break;
				}
								  //4. EXPIRE command ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case commands::EXPIRE: {
					std::string response;
					int iv;
					myDB::DataStore::DBVal val = r.value.value();
					if (std::holds_alternative<int>(val)) {
						 iv = std::get<int>(val);
						m_datastore.setExpiry(r.key, iv);
					}

					std::string logstr = "delete " + r.key;
					m_wal.writeLog(logstr);
					response = r.key + "is sheduled for expiration after " + std::to_string(iv) + " sec \n";
					SEND_TO_CLIENT(response)
				}
				}
			}
			else {
				LOG_ERROR("Wrong input")

			}
		}
	}
	closesocket(clientSocket);
	return EXIT_SUCCESS;
}

int TcpServer::startServer()
{
	if (initWinsock() != 0) return EXIT_FAILURE;
	if (makeListeningSocket() != 0) return EXIT_FAILURE;
	if (bindIPandPORT() != 0) return EXIT_FAILURE;
	if (startListining() != 0) return EXIT_FAILURE;
	m_wal.replayLog();

	while (true) {
		SOCKET clientSocket = accept(m_listining, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			LOG_ERROR("accept connection failed: " << WSAGetLastError());
			closesocket(m_listining);
			WSACleanup();
			return EXIT_FAILURE;
		}
		LOG_MSG("Client connected")
			std::string welcome = "Connected to server\n";
		send(clientSocket, welcome.c_str(), static_cast<int>(welcome.size()), 0);

		// capture 'this' and the local clientSocket by value
		m_threadPool.enqueue([this, clientSocket]() {
			this->handleClient(clientSocket);
			});
	};
	return 0;
}











