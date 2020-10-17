#pragma once

template<typename... ArgTypes>
void DrawAllInOrder(ArgTypes... args);

template<typename T, typename... ArgTypes>
inline void DrawAllInOrder(T* e, ArgTypes... args)
{
	e->Draw();
    DrawAllInOrder(args...);
}

template<typename T, typename... ArgTypes>
inline void DrawAllInOrder(std::vector<T*> v, ArgTypes... args)
{
	for (T* e : v) {
		e->Draw();
	}
    DrawAllInOrder(args...);
}

template<> 
inline void DrawAllInOrder() {
} 
