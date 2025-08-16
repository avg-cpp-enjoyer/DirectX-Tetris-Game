#pragma once

#include "Renderer.hpp"
#include "MainSceneBuilder.hpp"
#include "Scene.hpp"

class MainRenderer : public Renderer {
public:
	MainRenderer(HWND window, GameField& gameField, std::mutex& gfMutex);
	Scene& GetGameScene();
	Scene& GetUIScene();
	ActiveScene& GetActiveScene();
	void SetDropCallback(std::function<void()> callback);
	void UpdateAnimations(float deltaTime);
private:
	void RenderLoop() override;
	void RenderFrame() override;
private:
	GameField&            m_gameField;
	std::mutex&           m_gfMutex;
	Scene                 m_gameScene;
	Scene                 m_uiScene;
	ActiveScene           m_activeScene;
	std::function<void()> m_dropCallback;
};