#pragma once

#include <vector>
#include <functional>
#include <algorithm>

#define SUPPORT_MULTIPLE_SELFREGISTER_INHERITANCE false
#define ENABLE_OPTIMIZATION_BULK_DELETE_ALL false

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

	static void Sort(std::function<bool(T*, T*)> sortBy) {
		std::sort(GetAll().begin(), GetAll().end(), sortBy);
	}

#if SUPPORT_MULTIPLE_SELFREGISTER_INHERITANCE
	// This destructor doesn't need to be virtual if we guarantee the same object is never
	// on two different SelfRegister vectors (could happen with inheritance)
	virtual
#endif
	~SelfRegister()
	{
#if ENABLE_OPTIMIZATION_BULK_DELETE_ALL
		if (InBulkDelete()) return;
#endif
		GetAll().erase(std::remove(GetAll().begin(), GetAll().end(), (T*)this), GetAll().end());
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
#if ENABLE_OPTIMIZATION_BULK_DELETE_ALL
		InBulkDelete() = true;
#endif
		for (int i = GetAll().size()-1; i >= 0; i--)
		{
			T* e = GetAll()[i];
			delete e;
		}
#if ENABLE_OPTIMIZATION_BULK_DELETE_ALL
		GetAll().clear();
		InBulkDelete() = false;
#endif
	}

#if ENABLE_OPTIMIZATION_BULK_DELETE_ALL
private:
	static bool& InBulkDelete()
	{
		static bool running = false;
		return running;
	}
#endif
};
