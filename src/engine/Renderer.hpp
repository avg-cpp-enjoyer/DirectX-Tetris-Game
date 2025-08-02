#pragma once

#include "ui/Constants.hpp"
#include "model/GameField.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/GameComponents.hpp"

#include <d3d12.h>
#include <d2d1.h>
#include <d2d1_2.h>
#include <wrl/client.h>
#include <dcomp.h>
#include <mutex>
#include <functional>

class Renderer {
public:
	Renderer(ID2D1DeviceContext1* context, IDXGISwapChain* swapChain, IDCompositionDevice* dCompDevice);
	void Start();
	void Shutdown();
	Scene& GetGameScene();
	Scene& GetUIScene();
	void RenderLoop();
	void RegisterDropCallback(std::function<void()> callback);
	void AttachGameField(GameField& gameField, std::mutex& gfMutex);
	void UpdateAnimations(float deltaTime, GameField& gameField);
private:
	void RenderFrame() const;
private:
	ID2D1DeviceContext1*  m_context;
	IDXGISwapChain*       m_swapChain;
	IDCompositionDevice*  m_dCompDevice;

	GameField*            m_gameField = nullptr;
	std::mutex*           m_gfMutex = nullptr;
	std::function<void()> m_onDropComplete = nullptr;
	std::thread           m_thread;
	std::atomic<bool>     m_running{ false };

	Scene  m_gameScene;
	Scene  m_uiScene;
};