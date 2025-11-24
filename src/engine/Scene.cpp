#include "Scene.hpp"

#include <d2d1_2.h>
#include <ui/components/Component.hpp>

Scene::Scene(ID2D1DeviceContext1* context) : m_context(context) {}

void Scene::UpdateCommandLists() {
	m_buffer.ForEach([this](I2DGraphicsComponent* component) -> void {
		if (component->NeedsRedraw()) {
			component->ResetCommandList();
			m_context->SetTarget(component->GetCommandList());
			component->Draw();
			component->GetCommandList()->Close();
			component->SetRedraw(false);
		}
	});
}

void Scene::DrawAll() const {
	m_buffer.ForEach([this](const I2DGraphicsComponent* component) -> void {
		m_context->DrawImage(component->GetCommandList());
	});
}

void Scene::UpdateAll(float dt) {
	m_buffer.ForEach([dt](I2DGraphicsComponent* component) -> void {
		component->Update(dt);
	});
}

void Scene::Clear() {
	m_buffer.Clear();
	m_lookupTable.clear();
}