#include "utils.h"

namespace xbelikov {
	std::mutex guardForLog;

	void log(std::initializer_list<std::string> list) {
		std::lock_guard<std::mutex> lock(guardForLog);

		std::string temp = "";

		for (auto it = list.begin(); it != list.end(); it++) {
			if (it != list.begin()) {
				temp += " ";
			}

			temp += *it;
		}

		std::cout << temp << std::endl;
	}

	std::string thread_id_to_string(std::thread::id& id) {
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}
}