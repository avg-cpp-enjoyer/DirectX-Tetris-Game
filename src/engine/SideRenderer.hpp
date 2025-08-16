#pragma once

#include "Renderer.hpp"
#include "Scene.hpp"
#include "SideSceneBuilder.hpp"

class SideRenderer : public Renderer {
public:
	explicit SideRenderer(HWND window, int score);
	Scene& GetUIScene();
private:
	void RenderLoop() override;
	void RenderFrame() override;
private:
	Scene m_uiScene;
};