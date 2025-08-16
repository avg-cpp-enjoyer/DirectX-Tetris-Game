#include "Renderer.hpp"

Renderer::Renderer(HWND window) : m_renderTarget(window) {
	m_renderTarget.Initialize(GraphicsDevice::DXGIFactory(), GraphicsDevice::D3DDevice(),
		GraphicsDevice::D2DDevice(), GraphicsDevice::DCompDevice());
}

void Renderer::Start(int threadPriority, uintptr_t affinityMask) {
	if (!m_running.exchange(true)) {
		m_renderThread = std::thread(&Renderer::RenderLoop, this);
		SetThreadPriority(m_renderThread.native_handle(), threadPriority);
		SetThreadAffinityMask(m_renderThread.native_handle(), affinityMask);
	}
}

void Renderer::Shutdown() {
	m_running.store(false);
	if (m_renderThread.joinable()) {
		m_renderThread.join();
	}
}

RenderTarget& Renderer::GetTarget() {
	return m_renderTarget;
}
