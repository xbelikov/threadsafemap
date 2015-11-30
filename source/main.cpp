/**
 *	Тестовая программа
 *	проверка работоспособности потокобезопасной реализации Map
 */

#include <iostream>
#include <exception>
#include <thread>
#include <sstream>
#include "syncstringmap.h"

xbelikov::SyncStringMap m;

/**
 * Вставка с помощью структуры захвата WriteAccess 
 */
void addToMap1() {
	std::string tId = xbelikov::thread_id_to_string(std::this_thread::get_id());
	xbelikov::log({ "[Entered thread]", tId, "[Insert few items using WriterAccess]" });

	// Удобный вызов цепочкой
	m.getSyncWriteAccess()
		.set("123", "111")
		.set("234", "222")
		.set("456", "333")
		.set("567", "444")
		.remove("234")
		.remove("567");

		// в качестве альтернативы (например, если нужен цикл) можно делать так
		std::vector<xbelikov::vt> arr;

		arr.push_back({ "123", "999" });
		arr.push_back({ "234", "101" });
		arr.push_back({ "456", "110" });
		arr.push_back({ "567", "121" });

		// Выходя из области видимости будет вызван деструктор ~WriteAccess,
		// который снимет блокировку
		{
			xbelikov::WriteAccess wa = m.getSyncWriteAccess();

			for(auto item : arr) {
				wa.set(item.first, item.second);
			}

			wa.remove("456");
			wa.remove("567");
		}
	

	xbelikov::log({ "[Leaving thread]", tId });
}

/**
 * Альтернативная вставка с помощью вектора и метода SetFew
 */
void addToMap2() {
	std::string tId = xbelikov::thread_id_to_string(std::this_thread::get_id());
	xbelikov::log({ "[Entered thread]", tId, "[Insert few items using vector and setFew]" });

	std::vector<xbelikov::vt> tuple;

	tuple.push_back({ "123", "999" });
	tuple.push_back({ "234", "101" });
	tuple.push_back({ "456", "110" });
	tuple.push_back({ "567", "121" });

	m.setFew(tuple);

	xbelikov::log({ "[Leaving thread]", tId });
}

/**
 * Вставка стандартным методом Set, один вызов - одна блокировка
 */
void addToMap3() {
	std::string tId = xbelikov::thread_id_to_string(std::this_thread::get_id());
	xbelikov::log({ "[Entered thread] ", tId, "[Insert items using method set with single lock]" });
	
	m.set("123", "888");
	m.set("234", "777");
	m.set("456", "666");
	m.set("567", "555");
	
	xbelikov::log({ "[Leaving thread]", tId });
}

/**
 * Тестовый поиск, проверяем на месте ли всё
 */
void findInMap() {
	std::string tId = xbelikov::thread_id_to_string(std::this_thread::get_id());
	xbelikov::log({ "[Entered thread]", tId, "[Finding items in map]" });

	std::string ss[4] = { "123", "234", "456", "567" };
	
	for (auto s : ss) {
		auto it = m.find(s);
		if (it != m.end()) {
			xbelikov::log({ s, ": yep!", it->second });
		}
		else {
			xbelikov::log({ s, ": nope :(" });
		}
	}

	xbelikov::log({ "[Leaving thread]", tId });
}

/**
 * Чтение стандартной функцией Get, если элемента не было, то создатся новый
 */
void getFromMap() {
	xbelikov::log({ "[Read values by get method]" });
	xbelikov::log({ m.get("123") });
	xbelikov::log({ m.get("234") });
	xbelikov::log({ m.get("456") });
	xbelikov::log({ m.get("567") });

	//этого элемента нет в массиве
	xbelikov::log({ m.get("787") });
}

int main() {
	xbelikov::log({ "[Test App for my SyncStringMap implementation]" });
	xbelikov::log({ "[Start few threads and set some values]" });
	xbelikov::log({ "--------------------------------------------" });

	/**
	 * Создаём три потока для записи
	 */

	/* WriteAccess */
	std::thread t1(addToMap1);

	/* Set */
	std::thread t3(addToMap3);

	/* SetFew */
	std::thread t2(addToMap2);

	/**
	 * Не забываем подождать
	 */
	t1.join();
	t2.join();
	t3.join();

	/**
	 * Проверим на наличие
	 */
	findInMap();

	/**
	 * Достанем значения
	 */
	getFromMap();

	std::this_thread::sleep_for(std::chrono::seconds(120));
	return 0;
}