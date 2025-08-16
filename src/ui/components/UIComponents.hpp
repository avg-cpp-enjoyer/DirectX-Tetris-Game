#pragma once

#include "Component.hpp"
#include "ui/Constants.hpp"
#include "Button.hpp"
#include "engine/GraphicsDevice.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/RenderTarget.hpp"
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
	explicit BgComponent(const RenderTarget& target);
	void Draw() const override;
private:
	RectComponent m_bgRect;
};

class GridComponent : public Component {
public: 
	explicit GridComponent(const RenderTarget& target);
	void Draw() const override;
private:
	RectComponent m_gridRect;
};

class SideBgComponent : public Component {
public: 
	SideBgComponent(const RenderTarget& target, int score);
	void Draw() const override;
private:
	int m_score;
	ID2D1SolidColorBrush* m_textBrush;
	RectComponent m_bgRect;
};

class TitleBarComponent : public Component {
public:
	explicit TitleBarComponent(const RenderTarget& target);
	void Draw() const override;
private:
	ID2D1SolidColorBrush* m_uiBrush;
	ID2D1SolidColorBrush* m_borderBrush;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_path;
};

class PreviewComponent : public Component {
public:
	PreviewComponent(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
	RectComponent m_innerRect;
	RectComponent m_outerRect;
	ID2D1SolidColorBrush* m_textBrush;
};

class ScoreComponent : public Component {
public:
	ScoreComponent(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
	RectComponent m_rectComponent;
	ID2D1SolidColorBrush* m_textBrush;
};

class HighScoreComponent : public Component {
public:
	HighScoreComponent(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
	RectComponent m_rectComponent;
	ID2D1SolidColorBrush* m_textBrush;
};