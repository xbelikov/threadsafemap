#ifndef SYNCSTRINGMAP_H
#define SYNCSTRINGMAP_H

/**
 * Реализация потокобезопасного упорядоченного ассоциативного массива методом Read-Write Lock
 */

#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

#include "rwlock.h"
#include "utils.h"

namespace xbelikov {

	/** Упростим себе задачу - объявим несколько алиасов */

	/**
	 * Тип - константный итератор
	 */
	typedef std::map<std::string, std::string>::const_iterator stringmap_const_iterator;

	/**
	 * Тип - итератор
	 */
	typedef std::map<std::string, std::string>::iterator stringmap_iterator;

	/**
	 * Тип - упорядоченный ассоциативный массив где ключ и значение - строка
	 */
	typedef std::map<std::string, std::string> stringmap;

	/**
	 * Тип - пара ключ/значение используемого ассоциативного массива
	 */
	typedef stringmap::value_type vt;

	class SyncStringMap;

	/**
	 * Структура для захвата контроля над записью 
	 */
	struct WriteAccess {
		/**
		 * Единственный конструктор
		 * принимает ссылку на целевой объект
		 * лочит доступ к записи в целевой объект, 
		 * если доступ никто еще не захватил
		 * @param m целевой объект
		 */
		WriteAccess(SyncStringMap& m);

		/**
		 * Деструктор
		 * снимает блокировку, уведомляет ожидающих
		 */
		~WriteAccess();

		/**
		 * Удобный метод вставки
		 * возвращаемая ссылка на объект позволяет выстраивать цепочки вызовов внутри одной блокировки
		 * @param k ключ 
		 * @param v значение
		 */
		WriteAccess& set(const std::string& k, const std::string& v);

		/**
		 * Удобный метод удаления
		 * возвращаемая ссылка на объект позволяет выстраивать цепочки вызовов внутри одной блокировки
		 * @param k ключ
		 */
		WriteAccess& remove(const std::string& k);

	private:
		SyncStringMap& map;
	};



	/**
	 * Наша замечательная обёртка над std::map<std::string, std::string>
	 * (Слегка модифицировав код можно смело сделать шаблонный класс для чего угодно)
	 */
	class SyncStringMap {
	public:

		/**
		 * Подружим нашу обёртку и структуру для захвата
		 */
		friend WriteAccess;

		/**
		 * Получим с помощью данного метода структуру для захвата
		 */
		WriteAccess getSyncWriteAccess();

		/**
		 * Обычный метод поиска значения:
		 * поставит лок, найдёт и вернёт значение
		 * создаст элемент, если таковой не присутствует
		 * снимет лок по завершению операции
		 * @param k ключ
		 */
		std::string get(const std::string& k);

		/**
		 * Обычный метод удаления:
		 * поставит лок, удалит элемент
		 * снимет лок по завершению операции
		 * @param k ключ
		 */
		void remove(const std::string& k);

		/**
		* Обычный метод установки значения:
		* поставит лок, вставит или обновит элемент
		* снимет лок по завершению операции
		* @param k ключ
		* @param v значение
		*/
		void set(const std::string& k, const std::string& v);

		/**
		* Альтернативный способ установки нескольких значений за одну блокировку:
		* поставит лок, вставит или обновит пары из вектора
		* снимет лок по завершению операции
		* @param a массив пар ключ/значение
		*/
		void setFew(const std::vector<vt>& a);

		/**
		 * Безопасные, константные методы
		 */

		/**
		 * Поиск
		 * @param k ключ
 		 */
		stringmap_const_iterator find(const std::string& k) const;

		/**
		 * Первый элемент
		 */
		stringmap_const_iterator begin() const;

		/**
		 * Последний элемент
		 */
		stringmap_const_iterator end() const;

	private:

		/**
		* Пока нам в с++17 только обещают вставку с заменой
		* поэтому реализуем сами
		* @param pair пара ключ/значение
		*/
		void insert_or_assign(vt& pair);

		/**
		 * Хранилище
		 */
		stringmap storage;

		/**
		 * Умный блокировщик чтение/запись
		 */
		ReadWriteLock locker;
	};

}
#endif