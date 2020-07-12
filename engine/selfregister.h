#pragma once

#include <vector>
#include <functional>
#include <algorithm>

/**
 * Inheriting from this class gives you a static method GetAll()
 * that returns a set with all the current instances of the class.
 */
template <typename T>
class SelfRegister
{
public:
	SelfRegister()
	{
		GetAll().push_back((T*)this);
	}
	SelfRegister(const SelfRegister& other)
	{
		GetAll().push_back((T*)this);
	}
	void operator=(const SelfRegister& other)
	{
		GetAll().push_back((T*)this);
	}
	static std::vector<T*>& GetAll()
	{
		static std::vector<T*> instancies;
		return instancies;
	}
	virtual ~SelfRegister()
	{
		// This is quite inefficient becuase it has to walk the entire array to find the element and then compact it.
		// This could be done in a single pass in DeleteNotAlive/DeleteAll for all pointers, which already walks the array (and deletes from it, which calls this!), but only if:
		// (a) we never delete manually anywhere else (or we do so with a function that does call erase for that element like here) and
		// (b) the same object is never on two different SelfRegister vectors (could happen with inheritance)
		// If the (b) is true that means we can also make this destructor non-virtual and not force all SelfRegister classes to be polymorphic
		GetAll().erase(std::remove(GetAll().begin(), GetAll().end(), (T*)this), GetAll().end());
	}
	static void Sort(std::function<bool(T*,T*)> sortBy) {
		std::sort(GetAll().begin(), GetAll().end(), sortBy);
	}
	static void DeleteNotAlive()
	{
		for (int i = GetAll().size()-1; i >= 0; i--)
		{
			T* e = GetAll()[i];
			if (!e->alive)
			{
				delete e;
			}
		}
	}
	static void DeleteAll()
	{
		for (int i = GetAll().size()-1; i >= 0; i--)
		{
			T* e = GetAll()[i];
			delete e;
		}
	}
};
