#include "MainSceneBuilder.hpp"

MainSceneBuilder::MainSceneBuilder(
	const RenderTarget& target, ActiveScene& activeScene, Scene& gameScene, 
	Scene& uiScene, const GameField& field, HWND window
) : m_renderTarget(target), m_activeScene(activeScene), m_gameScene(gameScene), 
	m_uiScene(uiScene), m_gameField(field), m_window(window) {}

void MainSceneBuilder::BuildScene() {
	using namespace UI::MainWindow;
	using namespace UI::General;

	m_uiScene.Add(bgId, std::make_shared<BgComponent>(m_renderTarget));
	m_uiScene.Add(titleBarId, std::make_shared<TitleBarComponent>(m_renderTarget));
	m_uiScene.Add(gridId, std::make_shared<GridComponent>(m_renderTarget));
	m_uiScene.Add(scoreId, std::make_shared<ScoreComponent>(m_renderTarget, m_gameField));
	m_uiScene.Add(highScoreId, std::make_shared<HighScoreComponent>(m_renderTarget, m_gameField));
	m_uiScene.Add(previewId, std::make_shared<PreviewComponent>(m_renderTarget, m_gameField));

	m_uiScene.Add(pauseBtnId, std::make_shared<ButtonComponent>(m_window, L"Pause", pauseRect,
		uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));
	 
	m_uiScene.Add(quitBtnId, std::make_shared<ButtonComponent>(m_window, L"Quit", quitRect,
		uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));

	m_uiScene.Add(closeBtnId, std::make_shared<TitleButtonComponent>(m_window, L"✕", closeRect,
		UI::MainWindow::cornerRadius, true, textColor, uiColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));

	m_uiScene.Add(minimizeBtnId, std::make_shared<ButtonComponent>(m_window, L"⤵", minimizeRect,
		0.0f, true, textColor, uiColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));

	m_gameScene.Add(blockGridId, std::make_shared<BlockGridComponent>(m_renderTarget, m_gameField));
	m_gameScene.Add(ghostId, std::make_shared<GhostComponent>(m_renderTarget, m_gameField));
	m_activeScene.Add(std::make_shared<TetraminoComponent>(m_renderTarget, m_gameField));
}
