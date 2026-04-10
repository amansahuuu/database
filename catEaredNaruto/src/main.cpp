#include <iostream>
#include "store/DataStore.h"
#include "parser/parser.h"
#include "server/TcpServer.h"
#include "Welcome/welcome.h"
int main() {
	Welcome welcome;
	
	TcpServer ts;
	return ts.startServer();
}