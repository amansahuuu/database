

#include <sstream>
#include <vector>
#include <cctype>
#include <iostream>
#include "parser.h"
#include "../helpers/helper.h"

#define USER_INPUT std::vector<std::string>& user_input

//helper function to make strings uppercase
std::string toUpperCase(std::string& s) {
	for (char& c : s) {
		c = std::toupper(static_cast<unsigned char> (c));
	}
	return s;
}

std::optional<commands> getCommands(USER_INPUT)
{
	if (toUpperCase(user_input.at(0)) == "GET") return commands::GET;
	else if (toUpperCase(user_input.at(0)) == "SET") return commands::SET;
	else if (toUpperCase(user_input.at(0)) == "DELETE") return commands::DEL;
	else if (toUpperCase(user_input.at(0)) == "EXPIRE") return commands::EXPIRE;
	else {
		LOG_ERROR("Invalid command type\nuse only \"GET\", \"SET\" and \"DELETE\"")
			return std::nullopt;
	}
}
std::string getKey(USER_INPUT)
{
	return user_input.at(1);
}
DBVal getValue(USER_INPUT)
{
	const std::string& str = user_input.at(2); // value is the 3rd token (index 2)
	LOG_MSG("BEFORE TRY: user_input.at(2): " << str)

		// try parsing as integer
		try {
			std::size_t pos = 0;
			int ival = std::stoi(str, &pos);
			if (pos == str.length()) {
				LOG_MSG(ival)
					return ival;
			}
			// partial parse -> treat as non-int and try float
		}
	catch (const std::exception& e) {
		LOG_ERROR("conversion to int failed: " << e.what());
		// don't return here; fall through to try float
	}

	// try parsing as float
	try {
		std::size_t pos = 0;
		float fval = std::stof(str, &pos);
		if (pos == str.length()) {
			LOG_MSG(fval)
				return fval;
		}
		// partial parse -> treat as non-float and fall back to string
	}
	catch (const std::exception& e) {
		LOG_ERROR("conversion to float failed: " << e.what());
		// fall through to fallback to string
	}

	// fallback to string
	LOG_MSG(str)
		return DBVal(str);
}




std::optional<parsedInformation> Parser::parser(std::string& str) {
	if (str.empty()) return std::nullopt;
	std::vector<std::string> user_input;
	std::istringstream ss(str);
	std::string token;
	while (ss >> token) {
		user_input.push_back(token);
	}
	auto c = getCommands(user_input);
	if (c != std::nullopt) {
		if ((c == commands::SET || c == commands::EXPIRE) && user_input.size() == 3) {
			m_parsedInformation = { c, getKey(user_input), getValue(user_input) };
			return m_parsedInformation;
		}
		else if ((c == commands::DEL || c == commands::GET) && user_input.size() == 2) {
			m_parsedInformation = { c, getKey(user_input), std::nullopt };
			return m_parsedInformation;
		}
		return std::nullopt;
	}
	else
	{
		LOG_ERROR("SET has 3 arguments\n command(set), key and value")
			LOG_ERROR("DEL and GET has 2 argument\n command(del or get) and key")
			return std::nullopt;
		return std::nullopt;
	}

}

