#pragma once

#include <core/Base.hpp>
#include <engine/Renderer.hpp>
#include <cstdint>
#include <cstdarg>

class GameOverWindow : public Win32Base<GameOverWindow> {
public:
	explicit GameOverWindow(int score);
	~GameOverWindow() override {}

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
	bool ShouldRestart() const noexcept;
private:
	void OnCreate();
	void OnDestroy();
	intptr_t OnHitTest(intptr_t lParam) const;
private:
	Renderer m_renderer;
	int m_score;
	bool m_restart = false;
};