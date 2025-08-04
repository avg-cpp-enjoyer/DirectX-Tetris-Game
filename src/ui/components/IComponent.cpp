#include "IComponent.hpp"

Component::Component(const GraphicsDevice& device) : m_context(device.Context()) {
	m_context->CreateCommandList(&m_commandList);
}

ID2D1CommandList* Component::GetCommandList() const {
	return m_commandList.Get();
}

void Component::ResetCommandList() {
	m_commandList.Reset();
	m_context->CreateCommandList(&m_commandList);
}

void Component::SetRedraw(bool status) {
	m_needsRedraw = status; 
}

bool Component::NeedsRedraw() const {
	return m_needsRedraw;
}
