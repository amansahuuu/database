#include <thread>
#include <chrono>
#include "WAL.h"
#include "../helpers/helper.h"


WAL::WAL(Parser &parser, myDB::DataStore &datastore)
    : m_parser(parser), m_datastore(datastore), m_writeFile(m_logPath,  std::ios::app)
{
    LOG("Trying to open: " << m_logPath);
    LOG("File open result: " << m_writeFile.is_open());
    LOG("Failbit: " << m_writeFile.fail());
    LOG("Badbit: " << m_writeFile.bad());
    m_writeFile.open(m_logPath, std::ios::in | std::ios::out | std::ios::app);
    if (!m_writeFile) {
        // create file
        std::ofstream create(m_logPath);
        create.close();
        m_writeFile.open(m_logPath, std::ios::in | std::ios::out | std::ios::app);
    }
    if (!m_writeFile) {
        LOG_FATAL_ERROR("LOGFILE.TXT NOT GENERATED");
        for (int i = 5; i >= 1; i--) {
            LOG("PROGRAM WILL ABORT IN " << i << " SECONDS");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
       // abort();
    }
}

int WAL::writeLog(const std::string &command) {
    try {
        m_writeFile.clear(); // clear flags before writing
        m_writeFile.seekp(0, std::ios::end);
        m_writeFile << command << '\n';
        m_writeFile.flush();
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        LOG_ERROR(e.what());
        return EXIT_FAILURE;
    }
}

int WAL::replayLog() {
    // prepare stream for reading from beginning
    m_writeFile.clear();
    m_writeFile.seekg(0, std::ios::beg);

    std::string line;
    while (std::getline(m_writeFile, line)) {
        // trim trailing CR and LF
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        if (line.empty()) continue;

        std::optional<parsedInformation> result;
        try {
            // Parser expects a non-const reference
            result = m_parser.parser(line);
        } catch (const std::exception &e) {
            LOG_ERROR("parser failed with error: " << e.what());
            continue; // continue replaying remaining lines
        }

        if (!result.has_value()) continue;
        auto parsed = result.value();
        if (!parsed.cmd.has_value()) continue;

        switch (parsed.cmd.value()) {
        case commands::SET: {
            if (!parsed.value.has_value()) {
                LOG_ERROR("SET without value in WAL: " << line);
                break;
            }
            // make a local lvalue to satisfy DataStore::writeData(DBVal&)
            myDB::DataStore::DBVal val = parsed.value.value();
            m_datastore.writeData(parsed.key, val);
            break;
        }
        case commands::DEL: {
            m_datastore.deleteData(parsed.key);
            break;
        }
        case commands::GET: {
            // GET is read-only; no state change required when replaying WAL
            break;
        }
        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}


