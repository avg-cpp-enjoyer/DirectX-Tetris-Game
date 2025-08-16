#include "TetrisWindow.hpp"

TetrisWindow::TetrisWindow() {
	timeBeginPeriod(1);
	HR_LOG(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
	UI::Init();

	if (!Create(nullptr, WS_POPUP | WS_VISIBLE, WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, CW_USEDEFAULT, CW_USEDEFAULT,
		UI::MainWindow::mwWidth, UI::MainWindow::mwHeight)) {
		MessageBox(m_window, L"Error: Unable to create window", L"Fatal error", MB_OK);
		std::terminate();
	}
}

TetrisWindow::~TetrisWindow() noexcept {
	CoUninitialize();
	timeEndPeriod(1);
}

const wchar_t* TetrisWindow::ClassName() const {
	return L"TetrisWindowClass";
}

intptr_t TetrisWindow::HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) {
	switch (msg) {
	case WM_NCCALCSIZE:
		return 0;
	case WM_NCHITTEST:
		return OnNcHitTest(lParam);
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

void TetrisWindow::Exec(int cmdShow) {
	ShowWindow(m_window, cmdShow);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

intptr_t TetrisWindow::OnNcHitTest(intptr_t lParam) const {
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	ScreenToClient(m_window, &pt);

	if (pt.y < UI::MainWindow::TitleBar::tbHeight) {
		return HTCAPTION;
	}

	return DefWindowProc(m_window, WM_NCHITTEST, 0, lParam);
}

void TetrisWindow::OnKeyDown(uintptr_t key) {
	switch (key) {
	case VK_LEFT:  
		m_gameController.PushCommand(GameController::Command::MoveLeft); break;
	case VK_RIGHT: 
		m_gameController.PushCommand(GameController::Command::MoveRight); break;
	case VK_DOWN:  
		m_gameController.PushCommand(GameController::Command::MoveDown); break;
	case VK_UP:    
		m_gameController.PushCommand(GameController::Command::Rotate); break;
	case VK_SPACE:
		m_gameController.PushCommand(GameController::Command::HardDrop); break;
	}
}

bool TetrisWindow::CreateGameOverWindow() {
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

void TetrisWindow::OnCreate() {
	GraphicsDevice::Initialize();
	m_renderer = std::make_unique<MainRenderer>(m_window, m_gameController.GetGameField(), m_gameController.GetGfMutex());
	ResourceManager::Initialize(m_renderer->GetTarget().Context(), GraphicsDevice::WICFactory());

	m_gameController.SetGameOverCallback([this]() { PostMessageW(m_window, WM_APP_GAMEOVER, 0, 0); });
	m_gameController.SetUISceneCallback([this]() -> Scene& { return m_renderer->GetUIScene(); });
	m_gameController.SetGameSceneCallback([this]() -> Scene& { return m_renderer->GetGameScene(); });

	m_renderer->SetDropCallback(std::bind(&GameController::OnDrop, &m_gameController));
	m_renderer->Start(THREAD_PRIORITY_HIGHEST, 1ULL << 2);

	auto* pauseBtn = m_renderer->GetUIScene().GetById<ButtonComponent>(UI::MainWindow::pauseBtnId);
	pauseBtn->SetOnClick([this, pauseBtn]() { OnPause(pauseBtn); });
	m_renderer->GetUIScene().GetById<ButtonComponent>(UI::MainWindow::quitBtnId)->SetOnClick(
		[this]() { PostMessage(m_window, WM_DESTROY, 0, 0); });
	m_renderer->GetUIScene().GetById<ButtonComponent>(UI::MainWindow::closeBtnId)->SetOnClick(
		[this]() { PostMessage(m_window, WM_CLOSE, 0, 0); });
	m_renderer->GetUIScene().GetById<ButtonComponent>(UI::MainWindow::minimizeBtnId)->SetOnClick(
		[this]() { ShowWindow(m_window, SW_MINIMIZE); });

	m_gameController.Start();
}

void TetrisWindow::OnDestroy() {
	m_renderer->Shutdown();
	m_gameController.Shutdown();
	ResourceManager::Shutdown();
	PostQuitMessage(0);
}

void TetrisWindow::OnGameOver() {
	m_gameController.StopExecution();

	if (!CreateGameOverWindow()) {
		PostMessageW(m_window, WM_DESTROY, 0, 0);
	}

	m_gameController.Restart();
}

void TetrisWindow::OnPause(ButtonComponent* button) {
	if (m_gameController.PauseGame()) {
		button->SetText(L"Continue");
	} else {
		button->SetText(L"Pause");
	}
	m_gameController.ClearCommands();
}
