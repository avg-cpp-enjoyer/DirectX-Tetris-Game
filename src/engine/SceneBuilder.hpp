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

	std::unique_ptr<Button>  m_pauseButton;
	std::unique_ptr<Button>  m_quitButton;
	std::unique_ptr<Button>  m_minimizeButton;
	std::unique_ptr<Button>  m_closeButton;
};