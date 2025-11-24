#pragma once

#include <stack>
#include <vector>

template <typename... Ts>
struct MaxSizeOf;

template <typename T>
struct MaxSizeOf<T> {
	static constexpr size_t value = sizeof(T);
};

template <typename T, typename U, typename... Rest>
struct MaxSizeOf<T, U, Rest...> {
	static constexpr size_t value = sizeof(T) > MaxSizeOf<U, Rest...>::value 
		? sizeof(T) : MaxSizeOf<U, Rest...>::value;
};

template <typename... Ts>
struct MaxAlignOf;

template <typename T>
struct MaxAlignOf<T> {
	static constexpr size_t value = alignof(T);
};

template <typename T, typename U, typename... Rest>
struct MaxAlignOf<T, U, Rest...> {
	static constexpr size_t value = alignof(T) > MaxAlignOf<U, Rest...>::value 
		? alignof(T) : MaxAlignOf<U, Rest...>::value;
};

template <typename F, typename T>
concept ForeachFn = requires(F function, T* arg) {
	{ function(arg) } -> std::same_as<void>;
};

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
class PolymorphicBuffer {
public:
	PolymorphicBuffer(size_t capacity);
	~PolymorphicBuffer();

	template <typename U, typename... Args> requires std::is_base_of_v<T, U> &&
	(sizeof(U) <= maxSlotSize && alignof(U) <= maxSlotAlign)
	std::pair<size_t, U*> Allocate(Args&&... args);
	void Deallocate(T* comp);
	template <ForeachFn<T> F>
	void ForEach(F&& function);
	template <ForeachFn<T> F>
	void ForEach(F&& function) const;
	T* Get(size_t index);
	const T* Get(size_t index) const;
	void Clear();
private:
	std::vector<std::aligned_storage_t<maxSlotSize, maxSlotAlign>> m_storage;
	std::vector<uint8_t> m_busyIndices;
	std::stack<size_t> m_freedIndices;
	size_t m_objCount = 0ULL;
};

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::PolymorphicBuffer(size_t capacity) {
	m_storage.resize(capacity);
	m_busyIndices.resize(capacity, 0);
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::~PolymorphicBuffer() {
	for (size_t i = 0; i < m_storage.size(); i++) {
		if (m_busyIndices[i]) {
			T* obj = reinterpret_cast<T*>(&m_storage[i]);
			obj->~T();
		}
	}
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
template <typename U, typename... Args> requires std::is_base_of_v<T, U> &&
(sizeof(U) <= maxSlotSize && alignof(U) <= maxSlotAlign)
inline std::pair<size_t, U*> PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::Allocate(Args&&... args) {
	if (m_objCount == m_storage.size() && m_freedIndices.empty()) {
		throw std::bad_alloc();
	}

	size_t index;
	if (!m_freedIndices.empty()) {
		index = m_freedIndices.top();
		m_freedIndices.pop();
	} else {
		index = m_objCount++;
	}

	m_busyIndices[index] = 1;
	U* obj = new (&m_storage[index]) U(std::forward<Args>(args)...);
	return std::make_pair(index, obj);
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline void PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::Deallocate(T* obj) {
	if (!obj) {
		return;
	}

	obj->~T();

	uint8_t* base = reinterpret_cast<uint8_t*>(m_storage.data());
	size_t offset = reinterpret_cast<uint8_t*>(obj) - base;
	size_t index  = offset / maxSlotSize;

	m_freedIndices.push(index);
	m_busyIndices[index] = 0;
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
template <ForeachFn<T> F>
inline void PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::ForEach(F&& function) {
	for (size_t i = 0; i < m_storage.size(); i++) {
		if (m_busyIndices[i]) {
			function(reinterpret_cast<T*>(&m_storage[i]));
		}
	}
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
template <ForeachFn<T> F>
inline void PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::ForEach(F&& function) const {
	for (size_t i = 0; i < m_storage.size(); i++) {
		if (m_busyIndices[i]) {
			function(reinterpret_cast<const T*>(&m_storage[i]));
		}
	}
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline T* PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::Get(size_t index) {
	return reinterpret_cast<T*>(&m_storage[index]);
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline const T* PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::Get(size_t index) const {
	return reinterpret_cast<const T*>(&m_storage[index]);
}

template <typename T, size_t maxSlotSize, size_t maxSlotAlign> requires std::is_abstract_v<T>
inline void PolymorphicBuffer<T, maxSlotSize, maxSlotAlign>::Clear() {
	for (size_t i = 0; i < m_storage.size(); i++) {
		if (m_busyIndices[i]) {
			T* obj = reinterpret_cast<T*>(&m_storage[i]);
			obj->~T();
			m_busyIndices[i] = 0;
		}

		while (!m_freedIndices.empty()) {
			m_freedIndices.pop();
		}

		m_objCount = 0;
	}
}