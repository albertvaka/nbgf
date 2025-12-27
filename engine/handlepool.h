#pragma once

#include <vector>
#include <span>
#include <cassert> // for assert()

template<typename T>
struct HandlePool
{
	struct Handle {
		Handle() : id(size_t(-1)), generation(0) {};

		T& operator*() const {
			assert(IsValid() && "Dereferencing invalid handle");
			return *(HandlePool<T>::Get(*this));
		}

		T* operator->() const {
			assert(IsValid() && "Dereferencing invalid handle");
			return HandlePool<T>::Get(*this);
		}

		bool IsValid() const {
			return (id < generations.size() && generations[id] == generation);
		}

		bool IsNull() const {
			return id == size_t(-1);
		}

		explicit operator bool() const { return IsValid(); }

		static const Handle Null;

        friend std::ostream& operator<<(std::ostream& os, const Handle& rhs) {
            if (rhs.IsNull()) {
                return os << "NullHandle";
            } else if (!rhs.IsValid()) {
                return os << "InvalidHandle";
            } else {
                return os << "Handle(id=" << rhs.id << ", gen=" << rhs.generation << ")";
            }
		}
	private:
		size_t id;
		size_t generation;
		Handle(size_t i, size_t g) : id(i), generation(g) {}
		friend struct HandlePool<T>;
	};

	struct IndexedRef {
		IndexedRef(size_t index) : denseIndex(index) {}
		Handle GetHandle() const {
			size_t id = reverse[denseIndex];
			return Handle{ id, generations[id] };
		}

		// Transparent behavior as T&
		operator T&() const { return Get(); }
		T& operator*() const { return Get(); }
		T* operator->() const { return &Get(); }
	private:
		size_t denseIndex;
		T& Get() const {
			return HandlePool<T>::dense[denseIndex];
		}
	};

	struct Iterator {
		explicit Iterator(size_t index) : index(index) {}

		IndexedRef operator*() const {
			return IndexedRef{ index };
		}

		Iterator& operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return index != other.index;
		}
	private:
		size_t index;
	};

	struct AllRange {
		Iterator begin() const { return Iterator{ 0 }; }
		Iterator end() const { return Iterator{ static_cast<size_t>(dense.size()) }; }
		IndexedRef operator[](size_t index) const { return IndexedRef{ index }; }
		bool empty() const { return dense.empty(); }
		std::span<T> AsSpan() { return dense; }
		std::span<const T> AsSpan() const { return dense; }
		operator std::span<T>() { return AsSpan(); }
		operator std::span<const T>() const { return AsSpan(); }
	};

	template<typename... Args>
	static Handle Create(Args&&... args) {
		size_t id;
		if (!freeIds.empty()) {
			id = freeIds.back();
			freeIds.pop_back();
		} else {
			id = generations.size();
			generations.push_back(0);
			sparse.push_back(0);
		}

		size_t denseIndex = dense.size();

		dense.emplace_back(std::forward<Args>(args)...);

		sparse[id] = denseIndex;
		reverse.push_back(id);

		return {id, generations[id]};
	}

	static T* Get(const Handle& h) {
		if (!h) {
			return nullptr;
		}
		size_t denseIndex = sparse[h.id];
		return &dense[denseIndex];
	}

	static void Delete(const Handle& h) {
		if (!h) {
			return;
		}
		size_t denseIndex = sparse[h.id];
		size_t lastIndex = dense.size() - 1;

		// swap-and-pop to keep dense array packed
		if (denseIndex != lastIndex) {
			// we don't use swap because T may not be movable
			dense[denseIndex].~T();
			new (&dense[denseIndex]) T(std::move(dense[lastIndex]));

			size_t movedId = reverse[lastIndex];
			reverse[denseIndex] = movedId;
			sparse[movedId] = denseIndex;
		}

		dense.pop_back();
		reverse.pop_back();

		// stale old handles and recycle id
		generations[h.id]++;
		freeIds.push_back(h.id);
	}

	static void DeleteAll() {
		dense.clear();
		reverse.clear();

		freeIds.clear();
		freeIds.reserve(generations.size());
		for (size_t id = 0; id < generations.size(); ++id) {
			++generations[id];
			freeIds.push_back(id);
		}

		// keep sparse sized for all ids
		sparse.assign(generations.size(), 0u);
	}

	static void DeleteNotAlive() {
		// Iterate backwards to remain safe with swap-and-pop
		for (int32_t i = static_cast<int32_t>(dense.size()) - 1; i >= 0; --i) {
			if (!dense[i].alive) {
				const size_t id = reverse[i];
				Delete(Handle{ id, generations[id] });
			}
		}
	}

	static AllRange GetAll() { return {}; }
	static size_t Size() { return dense.size(); }

	static size_t MaxSimultaneousInstancesEverCreated() { return generations.size(); }

	static void Reserve(size_t capacity) {
		dense.reserve(capacity);
		sparse.reserve(capacity);
		reverse.reserve(capacity);
		generations.reserve(capacity);
		freeIds.reserve(capacity);
	}
private:
	static inline std::vector<T> dense;
	static inline std::vector<size_t> sparse;
	static inline std::vector<size_t> reverse;
	static inline std::vector<size_t> generations;
	static inline std::vector<size_t> freeIds;
};

template<typename T>
const typename HandlePool<T>::Handle
HandlePool<T>::Handle::Null;