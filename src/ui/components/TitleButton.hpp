#pragma once

#include "Button.hpp"

class TitleButtonComponent : public ButtonComponent {
public:
	TitleButtonComponent(
		HWND parent, const std::wstring& text, const D2D1_RECT_F& bounds, float cornerRadius,
		bool borderless, const D2D1_COLOR_F& textColor, const D2D1_COLOR_F& borderColor,
		const D2D1_COLOR_F& defaultColor, const D2D1_COLOR_F& clickedColor, const D2D1_COLOR_F& hoveredColor,
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat, const RenderTarget& target
	);

	~TitleButtonComponent() override;
	void Draw() const override;
private:
	Microsoft::WRL::ComPtr<ID2D1Factory>      m_factory;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_geometry;
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> m_sink;
};