#pragma once

#include "IComponent.hpp"
#include "ui/Constants.hpp"
#include "ui/Button.hpp"
#include "engine/GraphicsDevice.hpp"
#include "engine/ResourceManager.hpp"
#include "model/GameField.hpp"

class RectComponent {
public:
	RectComponent(
		ID2D1DeviceContext1* context, ID2D1SolidColorBrush* bgBrush, 
		ID2D1SolidColorBrush* borderBrush, const D2D1_ROUNDED_RECT& rect
	);
	void Draw() const;
private:
	ID2D1DeviceContext1* m_context;
	ID2D1SolidColorBrush* m_bgBrush;
	ID2D1SolidColorBrush* m_borderBrush;
	const D2D1_ROUNDED_RECT& m_rect;
};

class BgComponent : public Component {
public: 
	explicit BgComponent(const GraphicsDevice& device);
	void Draw() const override;
private:
	RectComponent m_bgRect;
};

class GridComponent : public Component {
public: 
	explicit GridComponent(const GraphicsDevice& device);
	void Draw() const override;
private:
	RectComponent m_gridRect;
};

class GOBgComponent : public RectComponent {
public: explicit GOBgComponent(const GraphicsDevice& device);
};

class TitleBarComponent : public Component {
public:
	TitleBarComponent(const GraphicsDevice& device);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_path;
};

class PreviewComponent : public Component {
public:
	PreviewComponent(const GameField& field, const GraphicsDevice& device);
	void Draw() const override;
private:
	const GameField& m_gameField;
	RectComponent m_innerRect;
	RectComponent m_outerRect;
	const GraphicsDevice& m_graphicsDevice;
};

class ScoreComponent : public Component {
public:
	ScoreComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	const GameField& m_gameField;
	RectComponent m_rectComponent;
};

class HighScoreComponent : public Component {
public:
	HighScoreComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	const GraphicsDevice& m_graphicsDevice;
	const GameField& m_gameField;
	RectComponent m_rectComponent;
};