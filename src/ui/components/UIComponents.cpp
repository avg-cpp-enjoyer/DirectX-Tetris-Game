#include "UIComponents.hpp"
#include "Component.hpp"

#include <d2d1_2.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <engine/RenderTarget.hpp>
#include <ui/Constants.hpp>
#include <model/GameField.hpp>
#include <cstdint>
#include <string>
#include <engine/GraphicsDevice.hpp>
#include <wrl/client.h>
#include <engine/ResourceManager.hpp>
#include <model/Tetramino.hpp>
#include <core/vec2.hpp>
#include <dcommon.h>

RectComponent::RectComponent(
	ID2D1DeviceContext1* context, ID2D1SolidColorBrush* bgBrush, 
	ID2D1SolidColorBrush* borderBrush, const D2D1_ROUNDED_RECT& rect
) : m_context(context), m_bgBrush(bgBrush), m_borderBrush(borderBrush), m_rect(rect) {}

void RectComponent::Draw() const {
	using UI::General::strokeWidth;
	static const float offset = strokeWidth * 0.5f;

	m_context->FillRoundedRectangle(m_rect, m_bgBrush);
	m_context->SetTransform(D2D1::Matrix3x2F::Translation(offset, offset));
	m_context->DrawRoundedRectangle(m_rect, m_borderBrush, strokeWidth);
	m_context->SetTransform(D2D1::Matrix3x2F::Identity());
}

BgComponent::BgComponent(const RenderTarget& target) : 
	I2DGraphicsComponent(target.Context()), m_bgRect(target.Context(), 
		target.GetBrush(RenderTarget::BrushType::Background), 
		target.GetBrush(RenderTarget::BrushType::Border), 
		UI::MainWindow::d2dWindowRect) 
{}

void BgComponent::Draw() const {
	m_bgRect.Draw();
}

GridComponent::GridComponent(const RenderTarget& target) : 
	I2DGraphicsComponent(target.Context()), m_gridRect(target.Context(),
		target.GetBrush(RenderTarget::BrushType::Background),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::MainWindow::d2dGameField) 
{}

void GridComponent::Draw() const {
	m_gridRect.Draw();
}

GameOverBgComponent::GameOverBgComponent(const RenderTarget& target, int score) : 
	I2DGraphicsComponent(target.Context()), m_score(score), 
	m_textBrush(target.GetBrush(RenderTarget::BrushType::Text)), m_bgRect(target.Context(),
		target.GetBrush(RenderTarget::BrushType::Background),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::GameOver::gameOverRect) 
{}

void GameOverBgComponent::Draw() const {
	m_bgRect.Draw();

	std::wstring text = L"Game Over!\nScore: " + std::to_wstring(m_score) 
		+ L"\nHigh Score: " + std::to_wstring(GameField::GetHighScore());

	m_context->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		GraphicsDevice::TextFormat(), UI::GameOver::textRect, m_textBrush);
}

TitleBarComponent::TitleBarComponent(const RenderTarget& target) : 
	I2DGraphicsComponent(target.Context()), 
	m_uiBrush(target.GetBrush(RenderTarget::BrushType::UI)), 
	m_borderBrush(target.GetBrush(RenderTarget::BrushType::Border)) {

	using UI::General::strokeWidth;
	using UI::General::scaleFactor;
	using UI::MainWindow::cornerRadius;
	using UI::MainWindow::mwWidth;
	using UI::MainWindow::TitleBar::tbHeight;

	static const float left    = strokeWidth;
	static const float top     = strokeWidth;
	static const float right   = mwWidth - 2.0f * scaleFactor;
	static const float bottom  = tbHeight + 2.0f * scaleFactor;

	Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;

	GraphicsDevice::Factory()->CreatePathGeometry(const_cast<ID2D1PathGeometry**>(m_path.GetAddressOf()));
	m_path->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(left, bottom), D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(D2D1::Point2F(left, top + cornerRadius));
	sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(left + cornerRadius, top), D2D1::SizeF(cornerRadius, cornerRadius),
		0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

	sink->AddLine(D2D1::Point2F(right - cornerRadius, top));
	sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(right, top + cornerRadius), D2D1::SizeF(cornerRadius, cornerRadius),
		0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

	sink->AddLine(D2D1::Point2F(right, bottom));
	sink->AddLine(D2D1::Point2F(left, bottom));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
	sink.Reset();
}

void TitleBarComponent::Draw() const {
	using UI::General::strokeWidth;
	using UI::General::scaleFactor;
	using UI::MainWindow::mwWidth;
	using UI::MainWindow::TitleBar::tbHeight;

	static const float right   = mwWidth - 2.0f * scaleFactor;
	static const float bottom  = tbHeight + 2.0f * scaleFactor;
	static const float offset  = strokeWidth * 0.5f;

	m_context->FillGeometry(m_path.Get(), m_uiBrush);
	m_context->SetTransform(D2D1::Matrix3x2F::Translation(offset, offset));
	m_context->DrawLine(D2D1::Point2F(0.0f, bottom), D2D1::Point2F(right, bottom), m_borderBrush, strokeWidth);
	m_context->SetTransform(D2D1::Matrix3x2F::Identity());
}

PreviewComponent::PreviewComponent(const RenderTarget& target, const GameField& field) :
	I2DGraphicsComponent(target.Context()), 
	m_gameField(field), 
	m_textBrush(target.GetBrush(RenderTarget::BrushType::Text)),
	m_outerRect(target.Context(),
		target.GetBrush(RenderTarget::BrushType::UI),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::MainWindow::d2dNextAreaOut),
	m_innerRect(target.Context(),
		target.GetBrush(RenderTarget::BrushType::Background),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::MainWindow::d2dNextAreaIn)
{}

void PreviewComponent::Draw() const {
	using namespace UI::MainWindow::GameField;
	using UI::MainWindow::Preview::offsets;
	using UI::MainWindow::d2dNextAreaIn;
	using UI::MainWindow::d2dNextAreaOut;

	m_outerRect.Draw();
	m_innerRect.Draw();

	m_context->DrawTextW(L"Next:", 5, GraphicsDevice::TextFormat(),
		D2D1::RectF(d2dNextAreaOut.rect.left, d2dNextAreaOut.rect.top,
			d2dNextAreaOut.rect.right, d2dNextAreaIn.rect.top), m_textBrush);

	const Tetramino& next = m_gameField.GetNextTetramino();
	ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(next.GetType());
	for (const auto& block : next.GetTetramino()) {
		auto it = offsets.find(next.GetType());
		float offX = it->second.x;
		float offY = it->second.y;

		float2 pos = next.GetPos() + block;
		D2D1_RECT_F dst = D2D1::RectF(
			d2dNextAreaIn.rect.left + offX + pos.x * blockSize,
			d2dNextAreaIn.rect.top + offY + pos.y * blockSize,
			d2dNextAreaIn.rect.left + offX + (pos.x + 1) * blockSize,
			d2dNextAreaIn.rect.top + offY + (pos.y + 1) * blockSize
		);
		m_context->DrawBitmap(bitmap, dst);
	}
}

ScoreComponent::ScoreComponent(const RenderTarget& target, const GameField& field) :
	I2DGraphicsComponent(target.Context()), m_gameField(field), m_textBrush(target.GetBrush(RenderTarget::BrushType::Text)),
	m_rectComponent(target.Context(),
		target.GetBrush(RenderTarget::BrushType::UI),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::MainWindow::d2dScoreRect) 
{}

void ScoreComponent::Draw() const {
	m_rectComponent.Draw();
	const std::wstring& text = L"Score: " + std::to_wstring(m_gameField.GetScore());
	m_context->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		GraphicsDevice::TextFormat(), UI::MainWindow::d2dScoreRect.rect, m_textBrush);
}

HighScoreComponent::HighScoreComponent(const RenderTarget& target, const GameField& field) :
	I2DGraphicsComponent(target.Context()), m_gameField(field), m_textBrush(target.GetBrush(RenderTarget::BrushType::Text)),
	m_rectComponent(target.Context(),
		target.GetBrush(RenderTarget::BrushType::UI),
		target.GetBrush(RenderTarget::BrushType::Border),
		UI::MainWindow::d2dHighRect) 
{}

void HighScoreComponent::Draw() const {
	m_rectComponent.Draw();
	const std::wstring& text = L"High: " + std::to_wstring(m_gameField.GetHighScore());
	m_context->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		GraphicsDevice::TextFormat(), UI::MainWindow::d2dHighRect.rect, m_textBrush);
}