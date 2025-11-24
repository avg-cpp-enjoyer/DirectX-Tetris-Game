#include "Component.hpp"

#include <d2d1_2.h>
#include <d2d1_1.h>

I2DGraphicsComponent::I2DGraphicsComponent(ID2D1DeviceContext1* context) : m_context(context) {
	m_context->CreateCommandList(&m_commandList);
}

ID2D1CommandList* I2DGraphicsComponent::GetCommandList() const {
	return m_commandList.Get();
}

void I2DGraphicsComponent::ResetCommandList() {
	m_commandList.Reset();
	m_context->CreateCommandList(&m_commandList);
}

void I2DGraphicsComponent::SetRedraw(bool status) {
	m_needsRedraw = status; 
}

bool I2DGraphicsComponent::NeedsRedraw() const {
	return m_needsRedraw;
}