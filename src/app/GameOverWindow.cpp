#include "GameOverWindow.hpp"

#include <Windows.h>
#include <cstdint>
#include <cstdarg>
#include <engine/RenderTarget.hpp>
#include <engine/GraphicsDevice.hpp>
#include <ui/components/UIComponents.hpp>
#include <ui/components/Button.hpp>

GameOverWindow::GameOverWindow(int score) : m_score(score) {}

const wchar_t* GameOverWindow::ClassName() const {
	return L"GameOverWindowClass";
}

intptr_t GameOverWindow::HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	switch (msg) {
	case WM_NCCALCSIZE:
		return 0;
	case WM_NCHITTEST:
		return OnHitTest(lParam);
	case WM_CREATE:
		OnCreate();
		break;
	case WM_DESTROY:
		OnDestroy();
		return 0;
	}

	return DefWindowProc(m_window, msg, wParam, lParam);
}

bool GameOverWindow::ShouldRestart() const noexcept {
	return m_restart;
}

void GameOverWindow::OnCreate() {
	using namespace UI::GameOver;
	using namespace UI::General;

	m_renderer.Initialize(m_window);

	m_renderer.GetTarget().AddBrush(UI::General::bgColor,     RenderTarget::BrushType::Background);
	m_renderer.GetTarget().AddBrush(UI::General::borderColor, RenderTarget::BrushType::Border);
	m_renderer.GetTarget().AddBrush(UI::General::textColor,   RenderTarget::BrushType::Text);

	m_renderer.GetScene().Add<GameOverBgComponent>(sideBgId, m_renderer.GetTarget(), m_score);
	m_renderer.GetScene().Add<ButtonComponent>(restartBtnId, m_window, L"Restart",
		restartRect, uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);
	m_renderer.GetScene().Add<ButtonComponent>(exitBtnId, m_window, L"Quit",
		quitRect, uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);
	m_renderer.GetScene().GetById<ButtonComponent>(UI::GameOver::restartBtnId)->SetOnClick(
		[this]() { m_restart = true; DestroyWindow(m_window); }
	);
	m_renderer.GetScene().GetById<ButtonComponent>(UI::GameOver::exitBtnId)->SetOnClick(
		[this]() { m_restart = false; DestroyWindow(m_window); }
	);

	m_renderer.Start(THREAD_PRIORITY_BELOW_NORMAL, 1ULL << 3);
}

void GameOverWindow::OnDestroy() {
	m_renderer.Shutdown();
}

intptr_t GameOverWindow::OnHitTest(intptr_t lParam) const {
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	ScreenToClient(m_window, &pt);

	return pt.y < UI::MainWindow::TitleBar::tbHeight ? HTCAPTION :
		DefWindowProc(m_window, WM_NCHITTEST, 0, lParam);
}