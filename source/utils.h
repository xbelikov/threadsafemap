#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <initializer_list>
#include <thread>
#include <string>
#include <sstream>
#include <mutex>

namespace xbelikov {
	void log(std::initializer_list<std::string> list);
	std::string thread_id_to_string(std::thread::id& id);
}

#endif