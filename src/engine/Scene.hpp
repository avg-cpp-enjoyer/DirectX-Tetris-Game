#pragma once

#include "ui/components/IComponent.hpp"

#include <vector>
#include <memory>

class Scene {
public:
	void Add(std::shared_ptr<IComponent> item);
	void DrawAll() const;
	void Clear();
private:
	std::vector<std::shared_ptr<IComponent>> m_items;
};