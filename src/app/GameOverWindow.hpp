#pragma once

#include "core/Base.hpp"
#include "model/GameField.hpp"
#include "ui/Constants.hpp"
#include "core/Log.hpp"
#include "engine/GraphicsDevice.hpp"
#include "ui/components/UIComponents.hpp"
#include "engine/SideRenderer.hpp"

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
	explicit GameOverWindow(int score);
	~GameOverWindow() override {}

	const wchar_t* ClassName() const override;
	intptr_t HandleMessage(uint32_t msg, uintptr_t wParam, intptr_t lParam) override;
	bool ShouldRestart() const noexcept;
private:
	void OnCreate();
	void OnDestroy();
	intptr_t OnNcHitTest(intptr_t lParam) const;
private:
	std::unique_ptr<SideRenderer> m_renderer;
	int m_score;
	bool m_restart = false;
};