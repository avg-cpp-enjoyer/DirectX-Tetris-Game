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

Component* Scene::GetComponentById(int id) {
	auto it = m_components.find(id);
	if (it != m_components.end()) {
		return it->second.get();
	}
	return nullptr;
}

void Scene::DrawAll() const {
	for (const auto& [id, component] : m_components) {
		m_context->DrawImage(component->GetCommandList());
	}
}

void Scene::Clear() {
	m_components.clear();
}

ActiveGameScene::ActiveGameScene(ID2D1DeviceContext1* context) : m_context(context) {}

void ActiveGameScene::Add(std::shared_ptr<Component> component) {
	m_components.push_back(std::move(component));
}

void ActiveGameScene::DrawAll() const {
	for (const auto& component : m_components) {
		component->Draw();
	}
}

void ActiveGameScene::Clear() {
	m_components.clear();
}