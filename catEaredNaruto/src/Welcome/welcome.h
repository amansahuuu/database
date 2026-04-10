#pragma once
#include <windows.h>
#include <iostream>

class Welcome {
public:
	Welcome();
	~Welcome();
    void printCommands();
	void printWelcomeMessage();
	void printInstructions();
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


};