#include "Renderer.hpp"

Renderer::Renderer(ID2D1DeviceContext1* context, IDXGISwapChain* swapChain, IDCompositionDevice* dCompDevice) 
	: m_context(context), m_swapChain(swapChain), m_dCompDevice(dCompDevice)
{}

void Renderer::Start() {
	if (m_running.exchange(true)) {
		return;
	}

	m_thread = std::thread(&Renderer::RenderLoop, this);

	SetThreadPriority(m_thread.native_handle(), THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(m_thread.native_handle(), 1ULL << 2);
}

void Renderer::Shutdown() {
	m_running.store(false);
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

Scene& Renderer::GetGameScene() {
	return m_gameScene;
}

Scene& Renderer::GetUIScene() {
	return m_uiScene;
}

void Renderer::RenderLoop() {
	auto lastTime = std::chrono::steady_clock::now();
	while (m_running.load(std::memory_order_relaxed)) {
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> deltaTime = now - lastTime;
		lastTime = now;
		{
			std::unique_lock<std::mutex> lock(*m_gfMutex);
			UpdateAnimations(static_cast<float>(deltaTime.count()), *m_gameField);
		}
		m_onDropComplete();
		RenderFrame();
	}
}

void Renderer::RegisterDropCallback(std::function<void()> callback) {
	m_onDropComplete = std::move(callback);
}

void Renderer::AttachGameField(GameField& gameField, std::mutex& gfMutex) {
	m_gameField = &gameField;
	m_gfMutex = &gfMutex;
}

void Renderer::UpdateAnimations(float deltaTime, GameField& gameField) {
	Tetramino& current = gameField.GetCurrentTetramino();
	Tetramino& ghost = gameField.GetGhostTetramino();
	current.UpdateAnimation(deltaTime);
	ghost.UpdateAnimation(deltaTime);
}

void Renderer::RenderFrame() const {
	m_context->BeginDraw();
	m_context->Clear(D2D1::ColorF(0, 0, 0, 0));

	m_uiScene.DrawAll();
	m_gameScene.DrawAll();

	m_context->EndDraw();
	m_swapChain->Present(1, 0);
	m_dCompDevice->Commit();
}
