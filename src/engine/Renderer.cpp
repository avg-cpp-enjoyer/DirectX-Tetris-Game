#include "Renderer.hpp"

Renderer::Renderer(
	ID2D1DeviceContext1* context, IDXGISwapChain* swapChain, IDCompositionDevice* dCompDevice
) : m_context(context), m_swapChain(swapChain), m_dCompDevice(dCompDevice), 
	m_gameScene(context), m_uiScene(context), m_activeScene(context) {
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;

	HR_LOG(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));

	D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	HR_LOG(m_context->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProps, &m_primaryBitmap));

	m_primaryTarget = m_primaryBitmap.Get();
	m_context->SetTarget(m_primaryTarget);
}

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

ActiveGameScene& Renderer::GetActiveGameScene() {
	return m_activeScene;
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

void Renderer::SetDropCallback(std::function<void()> callback) {
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

	m_uiScene.UpdateCommandLists();
	m_gameScene.UpdateCommandLists();

	m_context->SetTarget(m_primaryTarget);

	m_uiScene.DrawAll();
	m_gameScene.DrawAll();
	m_activeScene.DrawAll();

	m_context->EndDraw();
	m_swapChain->Present(1, 0);
	m_dCompDevice->Commit();
}
