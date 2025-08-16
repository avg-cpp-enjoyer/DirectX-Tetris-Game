#include "GameOverWindow.hpp"

GameOverWindow::GameOverWindow(int score) : m_score(score) {}

const wchar_t* GameOverWindow::ClassName() const {
	return L"GameOverWindowClass";
}

intptr_t GameOverWindow::HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	switch (msg) {
	case WM_NCCALCSIZE:
		return 0;
	case WM_NCHITTEST:
		return OnNcHitTest(lParam);
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
	m_renderer = std::make_unique<SideRenderer>(m_window, m_score);
	m_renderer->Start(THREAD_PRIORITY_BELOW_NORMAL, 1ULL << 3);

	m_renderer->GetUIScene().GetById<ButtonComponent>(UI::GameOver::restartBtnId)->SetOnClick(
		[this]() { m_restart = true; DestroyWindow(m_window); });
	m_renderer->GetUIScene().GetById<ButtonComponent>(UI::GameOver::exitBtnId)->SetOnClick(
		[this]() { m_restart = false; DestroyWindow(m_window); });
}

void GameOverWindow::OnDestroy() {
	m_renderer->Shutdown();
}

intptr_t GameOverWindow::OnNcHitTest(intptr_t lParam) const {
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	ScreenToClient(m_window, &pt);

	if (pt.y < UI::MainWindow::TitleBar::tbHeight) {
		return HTCAPTION;
	}

	return DefWindowProc(m_window, WM_NCHITTEST, 0, lParam);
}