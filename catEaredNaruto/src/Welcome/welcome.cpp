#include "welcome.h"
#include "../helpers/helper.h"



Welcome::Welcome()
{

	printWelcomeMessage();
	printInstructions();
	printCommands();
}
Welcome::~Welcome() {
	LOG_MSG("BYE")
}

void Welcome::printCommands()
{
	SetConsoleTextAttribute(hConsole, 6);
	LOG("SET has 3 arguments\n command(set), key and value")
	LOG("DEL and GET has 2 argument\n command(del or get) and key")
	SetConsoleTextAttribute(hConsole, 7);

}

void Welcome::printWelcomeMessage()
{
	SetConsoleTextAttribute(hConsole, 6);
	LOG_MSG("WELCOME TO MY FIRST INTENSIVE CPP PROGRAM")
	SetConsoleTextAttribute(hConsole, 7);
}

void Welcome::printInstructions()
{
	SetConsoleTextAttribute(hConsole, 10);
	LOG("this program runs on ip address 168.28.0.0:8080")
	SetConsoleTextAttribute(hConsole, 7);
}
