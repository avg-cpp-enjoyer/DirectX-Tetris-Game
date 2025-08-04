#include "SceneBuilder.hpp"

SceneBuilder::SceneBuilder(
	Renderer& renderer, GameController& controller, const GraphicsDevice& device, HWND window
) : m_renderer(renderer), m_gameController(controller), m_graphicsDevice(device), m_window(window) {}

void SceneBuilder::BuildUIScene() {
	using UI::MainWindow::cornerRadius;
	using namespace UI::MainWindow;
	using namespace UI::General;

	CreateButtons();
	CreateTitleButtons();

	m_renderer.GetUIScene().Add(bgId,        std::make_shared<BgComponent>(m_graphicsDevice));
	m_renderer.GetUIScene().Add(titleBarId,  std::make_shared<TitleBarComponent>(m_graphicsDevice));
	m_renderer.GetUIScene().Add(gridId,      std::make_shared<GridComponent>(m_graphicsDevice));
	m_renderer.GetUIScene().Add(scoreId,     std::make_shared<ScoreComponent>(m_graphicsDevice, m_gameController.GetGameField()));
	m_renderer.GetUIScene().Add(highScoreId, std::make_shared<HighScoreComponent>(m_graphicsDevice, m_gameController.GetGameField()));
	m_renderer.GetUIScene().Add(previewId,   std::make_shared<PreviewComponent>(m_gameController.GetGameField(), m_graphicsDevice));

	m_renderer.GetUIScene().Add(pauseBtnId,    m_pauseButton);
	m_renderer.GetUIScene().Add(quitBtnId,     m_quitButton);
	m_renderer.GetUIScene().Add(closeBtnId,    m_closeButton);
	m_renderer.GetUIScene().Add(minimizeBtnId, m_minimizeButton);
}

void SceneBuilder::BuildGameScene() {
	using namespace UI::MainWindow;

	m_renderer.GetGameScene().Add(blockGridId, std::make_shared<BlockGridComponent>(m_graphicsDevice, m_gameController.GetGameField()));
	m_renderer.GetGameScene().Add(ghostId, std::make_shared<GhostComponent>(m_graphicsDevice, m_gameController.GetGameField()));
	m_renderer.GetActiveGameScene().Add(std::make_shared<TetraminoComponent>(m_graphicsDevice, m_gameController.GetGameField()));
}

void SceneBuilder::CreateButtons() {
	using namespace UI::MainWindow;
	using namespace UI::General;

	m_pauseButton = std::make_shared<ButtonComponent>(m_window, L"Pause", pauseRect, 
		uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice);

	m_quitButton = std::make_shared<ButtonComponent>(m_window, L"Quit", quitRect, 
		uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice);

	m_pauseButton->SetOnClick([this]() {
		if (m_gameController.PauseGame()) {
			m_pauseButton->SetText(L"Continue");
		} else {
			m_pauseButton->SetText(L"Pause");
		}
		m_gameController.ClearCommands();
	});

	m_quitButton->SetOnClick([this]() {
		PostMessage(m_window, WM_DESTROY, 0, 0);
	});
}

void SceneBuilder::CreateTitleButtons() {
	using UI::MainWindow::cornerRadius;
	using namespace UI::MainWindow;
	using namespace UI::General;

	m_closeButton = std::make_shared<TitleButtonComponent>(m_window, L"✕", closeRect, 
		cornerRadius, true, textColor, uiColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice);

	m_minimizeButton = std::make_shared<ButtonComponent>(m_window, L"⤵", minimizeRect, 
		0.0f, true, textColor, uiColor, btnClrDefault,
		btnClrClicked, btnClrHovered, m_graphicsDevice.TextFormat(), m_graphicsDevice);

	m_closeButton->SetOnClick([this]() {
		PostMessage(m_window, WM_CLOSE, 0, 0);
	});

	m_minimizeButton->SetOnClick([this]() {
		ShowWindow(m_window, SW_MINIMIZE);
	});
}