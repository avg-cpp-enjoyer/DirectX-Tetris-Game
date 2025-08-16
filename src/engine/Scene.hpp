#pragma once

#include "ui/components/Component.hpp"

#include <unordered_map>
#include <memory>
#include <type_traits>

class Scene {
public:
	explicit Scene(ID2D1DeviceContext1* context);
	void Add(int id, std::shared_ptr<Component> component);
	void UpdateCommandLists() const;
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
	T* GetById(int id);
	void DrawAll() const;
	void Clear();
private:
	ID2D1DeviceContext1* m_context;
	std::unordered_map<int, std::shared_ptr<Component>> m_components;
};

template<typename T, typename>
inline T* Scene::GetById(int id) {
	auto it = m_components.find(id);
	if (it != m_components.end()) {
		return dynamic_cast<T*>(it->second.get());
	}
	return nullptr;
}

class ActiveScene {
public:
	explicit ActiveScene(ID2D1DeviceContext1* context);
	void Add(std::shared_ptr<Component> component);
	void DrawAll() const;
	void Clear();
private:
	ID2D1DeviceContext1* m_context;
	std::vector<std::shared_ptr<Component>> m_components;
};
