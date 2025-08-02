#include "GameOverWindow.hpp"

GameOverWindow::GameOverWindow(int score, int highScore) 
	: m_score(score), m_highScore(highScore) 
{}

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
		return 0;
	case WM_PAINT:
		RenderWindow();
		ValidateRect(m_window, nullptr);
		return 0;
	case WM_DESTROY:
		return 0;
	}

	return DefWindowProc(m_window, msg, wParam, lParam);
}

void GameOverWindow::OnCreate() {
	m_graphicsDevice.Initialize(m_window);
	m_bgComponent = std::make_unique<GOBgComponent>(m_graphicsDevice);

	CreateButtons();
	RenderWindow();
}

bool GameOverWindow::ShouldRestart() const noexcept {
	return m_restart;
}

intptr_t GameOverWindow::OnNcHitTest(intptr_t lParam) const {
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	ScreenToClient(m_window, &pt);

	if (pt.y < UI::MainWindow::TitleBar::tbHeight) {
		return HTCAPTION;
	}

	return DefWindowProc(m_window, WM_NCHITTEST, 0, lParam);
}

void GameOverWindow::CreateButtons() {
	using namespace UI::GameOver;
	using namespace UI::General;

	m_restartButton = std::make_unique<Button>(m_window, L"Restart", restartRect, uiCornerRad,
		false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice.Context()
	);

	m_quitButton = std::make_unique<Button>(m_window, L"Quit", quitRect, uiCornerRad,
		false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice.Context()
	);

	m_restartButton->SetOnClick([this]() {
		m_restart = true;
		DestroyWindow(m_window);
	});

	m_quitButton->SetOnClick([this]() {
		m_restart = false;
		DestroyWindow(m_window);
	});
}

void GameOverWindow::RenderWindow() {
	m_graphicsDevice.Context()->BeginDraw();
	m_graphicsDevice.Context()->Clear(D2D1::ColorF(0, 0, 0, 0));

	m_bgComponent->Draw();

	std::wstring text = L"Game Over!\nScore: " + std::to_wstring(m_score) + L"\nHigh Score: " + std::to_wstring(m_highScore);
	m_graphicsDevice.Context()->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		m_graphicsDevice.TextFormat(), UI::GameOver::textRect, 
		m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::Text));

	m_restartButton->Draw();
	m_quitButton->Draw();

	m_graphicsDevice.Context()->EndDraw();
	m_graphicsDevice.SwapChain()->Present(1, 0);
	m_graphicsDevice.DCompDevice()->Commit();
}