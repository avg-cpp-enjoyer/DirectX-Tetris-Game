#pragma once

#include <ui/components/Component.hpp>
#include <engine/RenderTarget.hpp>
#include <Windows.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <string>
#include <functional>
#include <d2d1.h>
#include <dcommon.h>
#include <cstdint>
#include <cstdarg>

class ButtonComponent : public I2DGraphicsComponent {
public:
	ButtonComponent(
		HWND parent, const std::wstring& text, const D2D1_RECT_F& bounds, float cornerRadius,
		bool borderless, const D2D1_COLOR_F& textColor, const D2D1_COLOR_F& borderColor,
		const D2D1_COLOR_F& defaultColor, const D2D1_COLOR_F& clickedColor, const D2D1_COLOR_F& hoveredColor,
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat, const RenderTarget& target
	);
	~ButtonComponent();

	HWND GetHandle() const noexcept;
	void SetText(const std::wstring& newText);
	void SetOnClick(std::function<void()> handler);
	virtual void Draw() const override;
private:
	void OnMouseMove(HWND window);
	void OnMouseLeave(HWND window);
	void OnLButtonDown(HWND window);
	void OnLButtonUp(HWND window);
	static intptr_t __stdcall ButtonProc(HWND window, uint32_t msg, uintptr_t wParam, intptr_t lParam);
protected:
	ID2D1SolidColorBrush* GetCurrentBrush() const;
protected:
	std::wstring m_text;
	D2D1_RECT_F m_bounds;
	D2D1_ROUNDED_RECT m_roundedRect;
	float m_cornerRadius;
	bool m_borderless;
	bool m_hovered = false;
	bool m_clicked = false;

	D2D1_COLOR_F m_defaultColor;
	D2D1_COLOR_F m_clickedColor;
	D2D1_COLOR_F m_hoveredColor;
	D2D1_COLOR_F m_borderColor;
	D2D1_COLOR_F m_textColor;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_defaultBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_hoveredBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_clickedBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_borderBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_textBrush;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>    m_textFormat;

	HWND m_button;
	WNDPROC m_oldProc;
	std::function<void()> m_onClick;
};