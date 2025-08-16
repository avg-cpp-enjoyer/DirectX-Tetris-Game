#include "SideSceneBuilder.hpp"

SideSceneBuilder::SideSceneBuilder(int score, const RenderTarget& target, Scene& scene, HWND window) 
	: m_score(score), m_renderTarget(target), m_scene(scene), m_window(window) {}

void SideSceneBuilder::BuildScene() {
	using namespace UI::GameOver;
	using namespace UI::General;

	m_scene.Add(sideBgId, std::make_shared<SideBgComponent>(m_renderTarget, m_score));
	m_scene.Add(restartBtnId, std::make_shared<ButtonComponent>(m_window, L"Restart", 
		restartRect, uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));

	m_scene.Add(exitBtnId, std::make_shared<ButtonComponent>(m_window, L"Quit", 
		quitRect, uiCornerRad, false, textColor, borderColor, btnClrDefault,
		btnClrClicked, btnClrHovered, GraphicsDevice::TextFormat(), m_renderTarget));
}
