#pragma once

#include "IComponent.hpp"
#include "ui/Constants.hpp"
#include "ui/Button.hpp"
#include "engine/GraphicsDevice.hpp"
#include "engine/ResourceManager.hpp"
#include "model/GameField.hpp"

class RectComponent : public IComponent {
public:
	RectComponent(
		ID2D1DeviceContext1* context, ID2D1SolidColorBrush* bgBrush, 
		ID2D1SolidColorBrush* borderBrush, const D2D1_ROUNDED_RECT& rect
	);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	ID2D1SolidColorBrush* m_bgBrush;
	ID2D1SolidColorBrush* m_borderBrush;
	const D2D1_ROUNDED_RECT& m_rect;
};

class BgComponent : public RectComponent {
public: explicit BgComponent(const GraphicsDevice& device);
};

class GridComponent : public RectComponent {
public: explicit GridComponent(const GraphicsDevice& device);
};

class GOBgComponent : public RectComponent {
public: explicit GOBgComponent(const GraphicsDevice& device);
};

class TitleBarComponent : public IComponent {
public:
	TitleBarComponent(const GraphicsDevice& device);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_path;
};

class PreviewComponent : public IComponent {
public:
	PreviewComponent(const GameField& field, const GraphicsDevice& device);
	void Draw() const override;
private:
	const GameField& m_gameField;
	RectComponent m_innerRect;
	RectComponent m_outerRect;
	const GraphicsDevice& m_graphicsDevice;
};

class ScoreComponent : public IComponent {
public:
	ScoreComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	const GameField& m_gameField;
	RectComponent m_rectComponent;
};

class HighScoreComponent : public IComponent {
public:
	HighScoreComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	const GameField& m_gameField;
	RectComponent m_rectComponent;
};

class ButtonComponent : public IComponent {
public:
	explicit ButtonComponent(Button* button);
	void Draw() const override;
private:
	Button* m_button = nullptr;
};