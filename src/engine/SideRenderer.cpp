#include "SideRenderer.hpp"

SideRenderer::SideRenderer(HWND window, int score) : Renderer(window), m_uiScene(m_renderTarget.Context()) {
	m_renderTarget.AddBrush(UI::General::bgColor,     RenderTarget::BrushType::Background);
	m_renderTarget.AddBrush(UI::General::borderColor, RenderTarget::BrushType::Border);
	m_renderTarget.AddBrush(UI::General::textColor,   RenderTarget::BrushType::Text);

	InitBuilder<SideSceneBuilder>(score, m_renderTarget, m_uiScene, window);
	m_builder->BuildScene();
}

Scene& SideRenderer::GetUIScene() {
	return m_uiScene;
}

void SideRenderer::RenderLoop() {
	while (m_running.load(std::memory_order_relaxed)) {
		RenderFrame();
	}
}

void SideRenderer::RenderFrame() {
	if (void* handle = m_renderTarget.FrameWaitHandle()) {
		WaitForSingleObjectEx(handle, 0xFFFFFFFF, true);
	}

	m_renderTarget.Context()->BeginDraw();
	m_renderTarget.Context()->Clear(D2D1::ColorF(0, 0, 0, 0));
	m_uiScene.UpdateCommandLists();
	m_renderTarget.SetTarget();

	m_uiScene.DrawAll();
	m_renderTarget.Present();
	GraphicsDevice::DCompDevice()->Commit();
}

