#include "rwlock.h"

namespace xbelikov {
	void ReadWriteLock::read_lock() {
		init_or_lock();
		readers++;
	}

	void ReadWriteLock::read_unlock() {
		readers--;
		locker.unlock();
	}

	void ReadWriteLock::write_lock() {
		init_or_lock();

		cv.wait(locker, [this]{ return (!writer || (readers > 0)); });
		writer = true;
	};

	void ReadWriteLock::write_unlock() {
		writer = false;
		locker.unlock();
		cv.notify_one();
	}

	void ReadWriteLock::init_or_lock() {
		if (!locker) {
			locker = std::unique_lock<std::mutex>(mutex);
		}
		else {
			locker.lock();
		}
	}
}