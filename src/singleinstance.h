#pragma once

#include <cassert>

#include <SFML/Graphics.hpp>

#include "partsys.h"

template <typename T>
class SingleInstance
{
public:
	static T*& instance()
	{
		static T* theOne = nullptr;
		return theOne;
	}
protected:
		SingleInstance()
		{
#ifdef _DEBUG
			assert(instance() == nullptr); //Creating a second instance of a singleton
#endif
			instance() = (T*)this;
		}
		~SingleInstance()
		{
			instance() = nullptr;
		}
private:
		SingleInstance(const SingleInstance& other) { }
		void operator=(const SingleInstance& other) { }

};

