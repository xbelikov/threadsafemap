#include "syncstringmap.h"

namespace xbelikov {
	WriteAccess::WriteAccess(
		SyncStringMap& m
		) : map(m) {

		map.locker.write_lock();
		log({ "[Write Access] lock" });
	};

	WriteAccess::~WriteAccess() {
		map.locker.write_unlock();
		log({ "[Write Access] unlock" });
	};

	WriteAccess& WriteAccess::set(const std::string& k, const std::string& v) {
		log({ "[Write Access] insert element ", k, v });
		map.insert_or_assign(vt(k, v));

		return (*this);
	}

	WriteAccess& WriteAccess::remove(const std::string& k) {
		log({ "[Write Access] remove element ", k });
		map.storage.erase(k);

		return (*this);
	};

	WriteAccess SyncStringMap::getSyncWriteAccess() {
		return WriteAccess(*this);
	}

	std::string SyncStringMap::get(const std::string& k) {
		std::string v = "";
		
		locker.read_lock();
		stringmap_const_iterator it = storage.find(k);
		
		if (it != storage.end()) {
			log({ "[Get]", k });
			v = it->second;
		}
		locker.read_unlock();

		if (v.empty()) {
			locker.write_lock();
			log({ "[Get] oops, key not found! Insert new:", k });
			storage.insert({ k, v });
			locker.write_unlock();
		}

		return v;
	};

	void SyncStringMap::remove(const std::string& k) {
		locker.write_lock();

		log({ "[Remove] ", k });
		storage.erase(k);

		locker.write_unlock();
	}

	void SyncStringMap::set(const std::string& k, const std::string& v) {
		locker.write_lock();

		log({ "[Set] insert", k, v });
		insert_or_assign(vt(k, v));

		locker.write_unlock();
	}

	void SyncStringMap::setFew(const std::vector<vt>& a) {
		locker.write_lock();

		for (auto item : a) {
			log({ "[SetFew] insert", item.first, item.second });
			insert_or_assign(item);
		}

		locker.write_unlock();
	}

	stringmap_const_iterator SyncStringMap::find(const std::string& k) const {
		return storage.find(k);
	}

	stringmap_const_iterator SyncStringMap::begin() const {
		return storage.begin();
	}

	stringmap_const_iterator SyncStringMap::end() const {
		return storage.end();
	}

	void SyncStringMap::insert_or_assign(vt& pair) {
		auto it = storage.find(pair.first);

		if (it == storage.end()) {
			storage.emplace(pair.first, std::move(pair.second));
		}
		else {
			it->second = std::move(pair.second);
		}
	};
}