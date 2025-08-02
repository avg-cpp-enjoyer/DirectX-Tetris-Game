#pragma once

#include "core/Base.hpp"
#include "ui/Constants.hpp"
#include "GameOverWindow.hpp"
#include "engine/Renderer.hpp"
#include "engine/GraphicsDevice.hpp"
#include "controller/GameController.hpp"
#include "engine/SceneBuilder.hpp"

#include <memory>

class TetrisWindow : public Base<TetrisWindow> {
public:
	TetrisWindow() = default;
	~TetrisWindow() noexcept override {}

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
private:
	GraphicsDevice                m_graphicsDevice;
	GameController                m_gameController;

	std::unique_ptr<SceneBuilder> m_builder;
	std::unique_ptr<Renderer>     m_renderer;

	static constexpr uint32_t     WM_APP_GAMEOVER = WM_APP + 1;
};