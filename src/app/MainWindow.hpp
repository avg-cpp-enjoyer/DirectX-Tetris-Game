#pragma once

#include <core/Base.hpp>
#include <engine/Renderer.hpp>
#include <controller/GameController.hpp>
#include <ui/components/Button.hpp>

#include <cstdint>
#include <cstdarg>
#include <Windows.h>

#pragma comment(lib, "Winmm.lib")

class MainWindow : public Win32Base<MainWindow> {
public:
	MainWindow();
	~MainWindow() noexcept override;

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
	int Exec(int cmdShow);
private:
	intptr_t OnHitTest(intptr_t lParam) const;
	void OnKeyDown(uintptr_t key);
	bool CreateGameOverWindow();
	void OnCreate();
	void OnDestroy();
	void OnGameOver();
	void OnPause(ButtonComponent* button);
private:
private:
	GameController m_gameController;
	Renderer m_renderer;
	static constexpr uint32_t WM_APP_GAMEOVER = WM_APP + 1;
};