#include "MainRenderer.hpp"

MainRenderer::MainRenderer(HWND window, GameField& gameField, std::mutex& gfMutex) : 
	Renderer(window), m_gameField(gameField), m_gfMutex(gfMutex), m_gameScene(m_renderTarget.Context()),
	m_uiScene(m_renderTarget.Context()), m_activeScene(m_renderTarget.Context()) {
	m_renderTarget.AddBrush(UI::General::bgColor,     RenderTarget::BrushType::Background);
	m_renderTarget.AddBrush(UI::General::uiColor,     RenderTarget::BrushType::UI);
	m_renderTarget.AddBrush(UI::General::textColor,   RenderTarget::BrushType::Text);
	m_renderTarget.AddBrush(UI::General::borderColor, RenderTarget::BrushType::Border);

	InitBuilder<MainSceneBuilder>(m_renderTarget, m_activeScene, m_gameScene, m_uiScene, gameField, window);
	m_builder->BuildScene();
}

Scene& MainRenderer::GetGameScene() {
	return m_gameScene;
}

Scene& MainRenderer::GetUIScene() {
	return m_uiScene;
}

ActiveScene& MainRenderer::GetActiveScene() {
	return m_activeScene;
}

void MainRenderer::SetDropCallback(std::function<void()> callback) {
	m_dropCallback = std::move(callback);
}

void MainRenderer::UpdateAnimations(float deltaTime) {
	Tetramino& current = m_gameField.GetCurrentTetramino();
	Tetramino& ghost = m_gameField.GetGhostTetramino();
	current.UpdateAnimation(deltaTime);
	ghost.UpdateAnimation(deltaTime);
}

void MainRenderer::RenderLoop() {
	auto lastTime = std::chrono::steady_clock::now();
	while (m_running.load(std::memory_order_relaxed)) {
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> deltaTime = now - lastTime;
		lastTime = now;
		{
			std::unique_lock<std::mutex> lock(m_gfMutex);
			UpdateAnimations(static_cast<float>(deltaTime.count()));
		}
		m_dropCallback();
		RenderFrame();
	}
}

void MainRenderer::RenderFrame() {
	if (void* handle = m_renderTarget.FrameWaitHandle()) {
		WaitForSingleObjectEx(handle, 0xFFFFFFFF, true);
	}

	m_renderTarget.Context()->BeginDraw();
	m_renderTarget.Context()->Clear(D2D1::ColorF(0, 0, 0, 0));
	m_uiScene.UpdateCommandLists();
	m_gameScene.UpdateCommandLists();
	m_renderTarget.SetTarget();

	m_uiScene.DrawAll();
	m_gameScene.DrawAll();
	m_activeScene.DrawAll();

	m_renderTarget.Present();
	GraphicsDevice::DCompDevice()->Commit();
}
