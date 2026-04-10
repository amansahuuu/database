#pragma once

#include <fstream>
#include <string>

#include "../parser/parser.h"
#include "../store/DataStore.h"

class WAL {
public:
    WAL(Parser& parser, myDB::DataStore& datastore);
    int writeLog(const std::string& command);
    int replayLog();
private:
    std::fstream m_writeFile;
    Parser& m_parser;
    myDB::DataStore& m_datastore;
const std::string m_logPath = "C:/Users/drama/wal.txt";
};
