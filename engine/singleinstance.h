#pragma once

#include "SDL_assert.h"

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
			SDL_assert(instance() == nullptr); //Creating a second instance of a singleton
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

