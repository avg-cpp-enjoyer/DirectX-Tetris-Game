#pragma once

#include "Renderer.hpp"
#include "controller/GameController.hpp"
#include "GraphicsDevice.hpp"
#include "ui/TitleButton.hpp"

class SceneBuilder {
public:
	SceneBuilder(Renderer& renderer, GameController& controller, const GraphicsDevice& device, HWND window);
	void BuildUIScene();
	void BuildGameScene();
private:
	void CreateButtons();
	void CreateTitleButtons();
private:
	Renderer&                m_renderer;
	GameController&          m_gameController;
	const GraphicsDevice&    m_graphicsDevice;
	HWND                     m_window;

	std::shared_ptr<ButtonComponent>  m_pauseButton;
	std::shared_ptr<ButtonComponent>  m_quitButton;
	std::shared_ptr<ButtonComponent>  m_minimizeButton;
	std::shared_ptr<ButtonComponent>  m_closeButton;
};