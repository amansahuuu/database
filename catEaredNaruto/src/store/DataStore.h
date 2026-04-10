#pragma once

#include <variant>
#include <optional>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <chrono>
#include <thread>
#include <vector>
using namespace std::string_literals;

namespace myDB {

	class DataStore {

	public:
		using DBVal = std::variant<int, float, std::string>;

		DataStore();
		~DataStore();
		std::optional<DBVal> readData(const std::string &key); // also for overriding
		void writeData(const std::string &key, DBVal &value);
		bool checkEntries(const std::string &key);
		bool deleteData(const std::string &key);
		bool setExpiry(const std::string& key, const unsigned int &time);
		
		

	private:
		std::unordered_map<std::string, DBVal> m_myDatabase;
		std::shared_mutex m_shared_mutex;
		std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_expiryMap;
		std::thread m_expiry_thread;
		std::atomic_bool m_stop_expiry_thread = false;
		std::mutex mutex_for_expiration;
	};

}

