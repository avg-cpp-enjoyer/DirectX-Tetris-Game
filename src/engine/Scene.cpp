#include "Scene.hpp"

Scene::Scene(ID2D1DeviceContext1* context) : m_context(context) {}

void Scene::Add(int id, std::shared_ptr<Component> component) {
	m_components.emplace(id, std::move(component));
}

void Scene::UpdateCommandLists() const {
	for (const auto& [id, component] : m_components) {
		if (component->NeedsRedraw()) {
			component->ResetCommandList();
			m_context->SetTarget(component->GetCommandList());
			component->Draw();
			component->GetCommandList()->Close();
			component->SetRedraw(false);
		}
	}
}

void Scene::DrawAll() const {
	for (const auto& [id, component] : m_components) {
		m_context->DrawImage(component->GetCommandList());
	}
}

void Scene::Clear() {
	m_components.clear();
}

ActiveScene::ActiveScene(ID2D1DeviceContext1* context) : m_context(context) {}

void ActiveScene::Add(std::shared_ptr<Component> component) {
	m_components.push_back(std::move(component));
}

void ActiveScene::DrawAll() const {
	for (const auto& component : m_components) {
		component->Draw();
	}
}

void ActiveScene::Clear() {
	m_components.clear();
}