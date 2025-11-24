#pragma once

#include "RenderTarget.hpp"
#include "Scene.hpp"

#include <thread>
#include <optional>
#include <Windows.h>
#include <atomic>
#include <cstdarg>

class Renderer {
public:
	Renderer() = default;
	Renderer(const Renderer&) = delete; 
	Renderer(Renderer&&) = delete;

	Renderer& operator=(const Renderer&) = delete; 
	Renderer& operator=(Renderer&&) = delete;

	void Initialize(HWND window);
	void Start(int threadPriority, uintptr_t affinityMask);
	void Shutdown();
	RenderTarget& GetTarget();
	Scene& GetScene();
private:
	void RenderLoop();
	void RenderFrame(float dt);
private:
	std::thread m_renderThread;
	std::atomic<bool> m_running{ false };
	RenderTarget m_renderTarget;
	std::optional<Scene> m_scene;
	bool m_initialized = false;
};