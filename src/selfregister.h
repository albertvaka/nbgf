#pragma once

#include <vector>
#include <functional>

/**
 * Inheriting from this class gives you a static method getAll()
 * that returns a set with all the current instances of the class.
 */
template <typename T>
class EntS
{
public:
	EntS()
	{
		getAll().push_back((T*)this);
	}
	EntS(const EntS& other)
	{
		getAll().push_back((T*)this);
	}
	void operator=(const EntS& other)
	{
		getAll().push_back((T*)this);
	}
	static std::vector<T*>& getAll()
	{
		static std::vector<T*> instancies;
		return instancies;
	}
	virtual ~EntS()
	{
		// This is quite inefficient becuase it has to walk the entire array to find the element and then compact it.
		// This could be done in a single pass in deleteNotAlive/deleteAll for all pointers, which already walks the array (and deletes from it, which calls this!), but only if:
		// (a) we never delete manually anywhere else (or we do so with a function that does call erase for that element like here) and
		// (b) the same object is never on two different EntS vectors (could happen with inheritance)
		// If the (b) is true that means we can also make this destructor non-virtual and not force all EntS classes to be polymorphic
		getAll().erase(std::remove(getAll().begin(), getAll().end(), (T*)this), getAll().end());
	}
	static void sort(std::function<bool(T*,T*)> sortBy) {
		std::sort(getAll().begin(), getAll().end(), sortBy);
	}
	static void deleteNotAlive()
	{
		for (int i = getAll().size()-1; i >= 0; i--)
		{
			T* e = getAll()[i];
			if (!e->alive)
			{
				delete e;
			}
		}
	}
	static void deleteAll()
	{
		for (int i = getAll().size()-1; i >= 0; i--)
		{
			T* e = getAll()[i];
			delete e;
		}
	}
};
