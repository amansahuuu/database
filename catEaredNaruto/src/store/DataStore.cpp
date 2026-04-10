
#include <optional>
#include <string>
#include <chrono>
#include "DataStore.h"
#include "../helpers/helper.h"


myDB::DataStore::DataStore() :m_stop_expiry_thread(false), m_expiry_thread([this]() {
	std::vector<std::string> toDelete;
	while (!m_stop_expiry_thread) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		
			auto now = std::chrono::high_resolution_clock::now();
			for (const auto& pair : m_expiryMap) {
				std::unique_lock <std::mutex> lock(mutex_for_expiration);
				if (pair.second <= now) {
					
					toDelete.push_back(pair.first);
				}
			}
			for (const auto& key : toDelete) {
				deleteData(key);
				m_expiryMap.erase(key);
				toDelete.pop_back();
			}
	}
	})
{}
myDB::DataStore::~DataStore()
{
	m_stop_expiry_thread = true;

	m_expiry_thread.join();
}

std::optional<myDB::DataStore::DBVal> myDB::DataStore::readData(const std::string& key) {
	std::shared_lock lock(m_shared_mutex);
	if (m_myDatabase.find(key) != m_myDatabase.end()) {
		return m_myDatabase.at(key);
	} else {
		return std::nullopt;
	}
}

void myDB::DataStore::writeData(const std::string& key, myDB::DataStore::DBVal& value) {
	std::unique_lock lock(m_shared_mutex); // placed here to prevent deadlock
	if (const auto* strPtr = std::get_if<std::string>(&value)) {
		if (strPtr->empty()) {
			LOG_ERROR("Cant add empty string");
			return;
		}
	}

	if (m_myDatabase.find(key) == m_myDatabase.end()) { // if key not found, then write data
		auto result = m_myDatabase.insert({ key, value });
		LOG_MSG("Key and value inserted :)")
	} else { // if key found, then override data
		if (m_myDatabase.at(key) != value) {
			m_myDatabase.at(key) = value;
			LOG_MSG("overriden " << key)
		} else {
			LOG_MSG("KEY HAS SAME VALUE")
		}
	}
}




bool myDB::DataStore::checkEntries(const std::string& key) {
	std::shared_lock lock(m_shared_mutex);
	if (m_myDatabase.find(key) != m_myDatabase.end()) return true; //found
	else return false; //not found
}

bool myDB::DataStore::deleteData(const std::string& key) {
	std::unique_lock lock(m_shared_mutex);
	if ((m_myDatabase.find(key) != m_myDatabase.end())) {
		
		m_myDatabase.erase(key);
        LOG_MSG("Key: " << key << " safely removed :)")
		return true;
	}
	else {
        LOG_ERROR("Key: " << key << " not found :(")
		return false;
	}
}

bool myDB::DataStore::setExpiry(const std::string& key, const unsigned int& time)
{
	std::shared_lock lock(m_shared_mutex);
	auto start = std::chrono::high_resolution_clock::now();
	auto deletionTime = start + std::chrono::seconds(time);
	//if this function is executed at 23:10, and value of time is 60 (seconds), then deletionTime should be 23:11
	m_expiryMap.insert({ key , deletionTime });
	return false;
}

