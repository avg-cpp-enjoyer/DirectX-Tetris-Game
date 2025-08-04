#pragma once

#include "core/Base.hpp"
#include "ui/Button.hpp"
#include "model/GameField.hpp"
#include "ui/Constants.hpp"
#include "core/Log.hpp"
#include "engine/GraphicsDevice.hpp"
#include "ui/components/UIComponents.hpp"

#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <memory>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

class GameOverWindow : public Base<GameOverWindow> {
public:
	GameOverWindow(int score, int highScore);
	~GameOverWindow() override {}

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
	void OnCreate();
	bool ShouldRestart() const noexcept;
private:
	void CreateButtons();
	void RenderWindow();
	intptr_t OnNcHitTest(intptr_t lParam) const;
private:
	int   m_score;
	int   m_highScore;
	bool  m_restart = false;

	GraphicsDevice                    m_graphicsDevice;
	std::unique_ptr<GOBgComponent>    m_bgComponent;
	std::unique_ptr<ButtonComponent>  m_restartButton;
	std::unique_ptr<ButtonComponent>  m_quitButton;
};