// Minimal types header: only the DBVal alias
#pragma once

#include <variant>
#include <string>

using DBVal = std::variant<int, float, std::string>;
