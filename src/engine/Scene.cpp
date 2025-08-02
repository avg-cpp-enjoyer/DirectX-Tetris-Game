#include "Scene.hpp"

void Scene::Add(std::shared_ptr<IComponent> item) {
	m_items.push_back(std::move(item));
}

void Scene::DrawAll() const {
	//TODO: Add 'needsRedraw' flag for each component so ui does not redraw every frame
	for (auto& item : m_items) {
		item->Draw();
	}
}

void Scene::Clear() {
	m_items.clear();
}