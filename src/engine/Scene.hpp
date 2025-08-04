#pragma once

#include "ui/components/IComponent.hpp"

#include <unordered_map>
#include <memory>

class Scene {
public:
	explicit Scene(ID2D1DeviceContext1* context);
	void Add(int id, std::shared_ptr<Component> component);
	void UpdateCommandLists() const;
	Component* GetComponentById(int id);
	void DrawAll() const;
	void Clear();
private:
	ID2D1DeviceContext1* m_context;
	std::unordered_map<int, std::shared_ptr<Component>> m_components;
};

class ActiveGameScene {
public:
	explicit ActiveGameScene(ID2D1DeviceContext1* context);
	void Add(std::shared_ptr<Component> component);
	void DrawAll() const;
	void Clear();
private:
	ID2D1DeviceContext1* m_context;
	std::vector<std::shared_ptr<Component>> m_components;
};