#include "Renderer.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"
#include "GraphicsDevice.hpp"

#include <core/Utils.hpp>
#include <thread>
#include <Windows.h>
#include <cstdarg>

void Renderer::Initialize(HWND window) {
	m_renderTarget.Initialize(window, GraphicsDevice::DXGIFactory(), GraphicsDevice::D3DDevice(),
		GraphicsDevice::D2DDevice(), GraphicsDevice::DCompDevice());
	m_scene.emplace(m_renderTarget.Context());
	m_initialized = true;
}

void Renderer::Start(int threadPriority, uintptr_t affinityMask) {
	ASSERT_TRUE(m_initialized, "Renderer not inititalized");	

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

Scene& Renderer::GetScene() {
	return *m_scene;
}

void Renderer::RenderLoop() {
	auto lastTime = std::chrono::steady_clock::now();

	while (m_running.load(std::memory_order::relaxed)) {
		auto now = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		RenderFrame(dt);
	}
}

void Renderer::RenderFrame(float dt) {
	m_renderTarget.BeginRender();
	m_scene->UpdateAll(dt);
	m_scene->UpdateCommandLists();
	m_renderTarget.SetTarget();
	m_scene->DrawAll();
	m_renderTarget.EndRender();
}

