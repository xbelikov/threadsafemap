#ifndef RW_LOCK_H
#define RW_LOCK_H

/**
 * Реализация Read-Write Lock для С++11
 */

#include <mutex>
#include <condition_variable>

namespace xbelikov {

	class ReadWriteLock {
	public:
		ReadWriteLock() : readers(0), writer(false) {};

		void read_lock();
		void read_unlock();

		/**
		 * Установить блокировку на запись
		 */
		void write_lock();
		
		/**
		* Снять блокировку на запись
		*/
		void write_unlock();

	private:
		/**
		 * Структуры для блокировки (С++11)
		 * Так как ожидание условия и нотификация 
		 * у нас реализованы с помощью condition_variable,
		 * то в качестве блокировщика используем unique_lock<mutex> 
		 * (recursive_mutex использовать не получится)
		 */
		std::unique_lock<std::mutex> locker;
		std::condition_variable cv;
		std::mutex mutex;

		/**
		 * Учет писателей и читателей
		 */
		bool writer;
		int readers;

		void init_or_lock();
	};
}
#endif