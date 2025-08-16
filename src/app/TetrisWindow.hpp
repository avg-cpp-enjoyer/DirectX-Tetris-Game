#pragma once

#include "core/Base.hpp"
#include "ui/Constants.hpp"
#include "GameOverWindow.hpp"
#include "engine/MainRenderer.hpp"
#include "engine/SideRenderer.hpp"
#include "engine/GraphicsDevice.hpp"
#include "controller/GameController.hpp"

#include <memory>

#pragma comment(lib, "Winmm.lib")

class TetrisWindow : public Base<TetrisWindow> {
public:
	TetrisWindow();
	~TetrisWindow() noexcept override;

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
	void Exec(int cmdShow);
private:
	intptr_t OnNcHitTest(intptr_t lParam) const;
	void OnKeyDown(uintptr_t key);
	bool CreateGameOverWindow();
	void OnCreate();
	void OnDestroy();
	void OnGameOver();
	void OnPause(ButtonComponent* button);
private:
	GameController                 m_gameController;
	std::unique_ptr<MainRenderer>  m_renderer;
	static constexpr uint32_t      WM_APP_GAMEOVER = WM_APP + 1;
};