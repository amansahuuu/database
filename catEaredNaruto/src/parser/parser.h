#pragma once

#include <optional>
#include <string>

#include "../types/types.h"

enum class commands {
	SET,
	GET,
	DEL,
	EXPIRE
};

struct parsedInformation {
	std::optional<commands> cmd;
	std::string key;
	std::optional<DBVal> value;
	
};

class Parser {
public:
	std::optional<parsedInformation> parser(std::string &str);

private:
	parsedInformation m_parsedInformation;
};
