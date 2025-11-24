#include "MainWindow.hpp"
#include "GameOverWindow.hpp"

#include <core/Utils.hpp>
#include <engine/Scene.hpp>
#include <engine/RenderTarget.hpp>
#include <engine/GraphicsDevice.hpp>
#include <engine/ResourceManager.hpp>
#include <ui/Constants.hpp>
#include <ui/components/UIComponents.hpp>
#include <ui/components/GameComponents.hpp>
#include <ui/components/Button.hpp>
#include <ui/components/TitleButton.hpp>

#include <Windows.h>
#include <exception>
#include <cstdint>
#include <cstdarg>

MainWindow::MainWindow() {
	timeBeginPeriod(1);
	HR_LOG(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
	UI::Init();

	if (!Create(nullptr, WS_POPUP | WS_VISIBLE, WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, CW_USEDEFAULT, CW_USEDEFAULT,
		UI::MainWindow::mwWidth, UI::MainWindow::mwHeight)) {
		MessageBox(m_window, L"Error: Unable to create window", L"Fatal error", MB_OK);
		std::terminate();
	}
}

MainWindow::~MainWindow() noexcept {
	CoUninitialize();
	timeEndPeriod(1);
}

const wchar_t* MainWindow::ClassName() const {
	return L"TetrisWindowClass";
}

intptr_t MainWindow::HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	switch (msg) {
	case WM_NCCALCSIZE:
		return 0;
	case WM_NCHITTEST:
		return OnHitTest(lParam);
	case WM_CREATE:
		OnCreate();
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;
	case WM_APP_GAMEOVER:
		OnGameOver();
		return 0;
	case WM_DESTROY:
		OnDestroy();
	}
	return DefWindowProc(m_window, msg, wParam, lParam);
}

int MainWindow::Exec(int cmdShow) {
	ShowWindow(m_window, cmdShow);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

intptr_t MainWindow::OnHitTest(intptr_t lParam) const {
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	ScreenToClient(m_window, &pt);

	return pt.y < UI::MainWindow::TitleBar::tbHeight ? HTCAPTION :
		DefWindowProcW(m_window, WM_NCHITTEST, 0, lParam);
}

void MainWindow::OnKeyDown(uintptr_t key) {
	switch (key) {
	case VK_LEFT:  m_gameController.ExecuteCommand(GameController::Command::MoveLeft);  break;
	case VK_RIGHT: m_gameController.ExecuteCommand(GameController::Command::MoveRight); break;
	case VK_DOWN:  m_gameController.ExecuteCommand(GameController::Command::MoveDown);  break;
	case VK_UP:    m_gameController.ExecuteCommand(GameController::Command::Rotate);    break;
	case VK_SPACE: m_gameController.ExecuteCommand(GameController::Command::HardDrop);  break;
	}
}

bool MainWindow::CreateGameOverWindow() {
	using UI::GameOver::goWidth;
	using UI::GameOver::goHeight;

	RECT rcMain;
	GetWindowRect(m_window, &rcMain);

	const int x = rcMain.left + (rcMain.right - rcMain.left - static_cast<int>(goWidth)) / 2;
	const int y = rcMain.top + (rcMain.bottom - rcMain.top - static_cast<int>(goHeight)) / 2;

	GameOverWindow gameOver(m_gameController.GetGameField().GetScore());
	gameOver.Create(L"Game Over", WS_POPUP | WS_VISIBLE, 0, x, y, static_cast<int>(goWidth), static_cast<int>(goHeight));

	EnableWindow(m_window, false);

	ShowWindow(gameOver.Window(), SW_SHOW);
	UpdateWindow(gameOver.Window());


	MSG msg;
	while (IsWindowVisible(gameOver.Window()) && GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	EnableWindow(m_window, true);
	SetForegroundWindow(m_window);

	return gameOver.ShouldRestart();
}

void MainWindow::OnCreate() {
	using namespace UI::MainWindow;
	using namespace UI::General;

	GraphicsDevice::Initialize();
	m_renderer.Initialize(m_window);
	ResourceManager::Initialize(m_renderer.GetTarget().Context(), GraphicsDevice::WICFactory());

	m_gameController.SetGameOverCallback([this]() -> void { PostMessageW(m_window, WM_APP_GAMEOVER, 0, 0); });
	m_gameController.SetSceneCallback([this]() -> Scene& { return m_renderer.GetScene(); });

	m_renderer.GetTarget().AddBrush(UI::General::bgColor,      RenderTarget::BrushType::Background);
	m_renderer.GetTarget().AddBrush(UI::General::uiColor,      RenderTarget::BrushType::UI);
	m_renderer.GetTarget().AddBrush(UI::General::textColor,    RenderTarget::BrushType::Text);
	m_renderer.GetTarget().AddBrush(UI::General::borderColor,  RenderTarget::BrushType::Border);

	m_renderer.GetScene().Add<BgComponent>          (bgId,        m_renderer.GetTarget());
	m_renderer.GetScene().Add<TitleBarComponent>    (titleBarId,  m_renderer.GetTarget());
	m_renderer.GetScene().Add<GridComponent>        (gridId,      m_renderer.GetTarget());
	m_renderer.GetScene().Add<ScoreComponent>       (scoreId,     m_renderer.GetTarget(), m_gameController.GetGameField());
	m_renderer.GetScene().Add<HighScoreComponent>   (highScoreId, m_renderer.GetTarget(), m_gameController.GetGameField());
	m_renderer.GetScene().Add<PreviewComponent>     (previewId,   m_renderer.GetTarget(), m_gameController.GetGameField());
	m_renderer.GetScene().Add<GameFieldView>        (blockGridId, m_renderer.GetTarget(), m_gameController.GetGameField());
	m_renderer.GetScene().Add<GhostTetraminoView>   (ghostId,     m_renderer.GetTarget(), m_gameController.GetGameField());
	m_renderer.GetScene().Add<CurrentTetraminoView> (tetraminoId, m_renderer.GetTarget(), m_gameController.GetGameField(), 
		                                                                                  m_gameController.GetGfMutex());

	m_renderer.GetScene().Add<ButtonComponent>(pauseBtnId, m_window, L"Pause", pauseRect,
		uiCornerRad, false, textColor, borderColor, btnClrDefault, btnClrClicked, 
		btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);
	m_renderer.GetScene().Add<ButtonComponent>(quitBtnId, m_window, L"Quit", quitRect,
		uiCornerRad, false, textColor, borderColor, btnClrDefault, btnClrClicked, 
		btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);
	m_renderer.GetScene().Add<TitleButtonComponent>(closeBtnId, m_window, L"✕", closeRect,
		UI::MainWindow::cornerRadius, true, textColor, uiColor, btnClrDefault, btnClrClicked, 
		btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);
	m_renderer.GetScene().Add<ButtonComponent>(minimizeBtnId, m_window, L"⤵", minimizeRect,
		0.0f, true, textColor, uiColor, btnClrDefault, btnClrClicked, 
		btnClrHovered, GraphicsDevice::TextFormat(), m_renderer.GetTarget()
	);

	auto* pauseBtn = m_renderer.GetScene().GetById<ButtonComponent>(UI::MainWindow::pauseBtnId);
	pauseBtn->SetOnClick([this, pauseBtn]() -> void { OnPause(pauseBtn); });

	m_renderer.GetScene().GetById<ButtonComponent>(UI::MainWindow::quitBtnId)->SetOnClick(
		[this]() -> void { PostMessage(m_window, WM_DESTROY, 0, 0); }
	);
	m_renderer.GetScene().GetById<ButtonComponent>(UI::MainWindow::closeBtnId)->SetOnClick(
		[this]() -> void { PostMessage(m_window, WM_CLOSE, 0, 0); }
	);
	m_renderer.GetScene().GetById<ButtonComponent>(UI::MainWindow::minimizeBtnId)->SetOnClick(
		[this]() -> void { ShowWindow(m_window, SW_MINIMIZE); }
	);

	m_renderer.Start(THREAD_PRIORITY_NORMAL, 1ULL << 2);
	m_gameController.Start(THREAD_PRIORITY_NORMAL, 1ULL << 4);
}

void MainWindow::OnDestroy() {
	m_renderer.Shutdown();
	m_gameController.Shutdown();
	ResourceManager::Shutdown();
	PostQuitMessage(0);
}

void MainWindow::OnGameOver() {
	m_gameController.StopExecution();

	if (!CreateGameOverWindow()) {
		PostMessageW(m_window, WM_DESTROY, 0, 0);
	}

	m_gameController.Restart();
}

void MainWindow::OnPause(ButtonComponent* button) {
	if (m_gameController.PauseGame()) {
		button->SetText(L"Continue");
	} else {
		button->SetText(L"Pause");
	}
}
