#include "UIComponents.hpp"

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

BgComponent::BgComponent(const GraphicsDevice& device)
	: RectComponent(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::Background),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dWindowRect) 
{}

GridComponent::GridComponent(const GraphicsDevice& device)
	: RectComponent(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::Background),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dGameField) 
{}

GOBgComponent::GOBgComponent(const GraphicsDevice& device) 
	: RectComponent(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::Background),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::GameOver::gameOverRect) 
{}

TitleBarComponent::TitleBarComponent(const GraphicsDevice& device) : m_graphicsDevice(device) {
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

	m_graphicsDevice.Factory()->CreatePathGeometry(const_cast<ID2D1PathGeometry**>(m_path.GetAddressOf()));
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

	m_graphicsDevice.Context()->FillGeometry(m_path.Get(), m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::UI));
	m_graphicsDevice.Context()->SetTransform(D2D1::Matrix3x2F::Translation(offset, offset));
	m_graphicsDevice.Context()->DrawLine(D2D1::Point2F(0.0f, bottom), D2D1::Point2F(right, bottom),
		m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::Border), strokeWidth);

	m_graphicsDevice.Context()->SetTransform(D2D1::Matrix3x2F::Identity());
}

PreviewComponent::PreviewComponent(const GameField& field, const GraphicsDevice& device) :
	m_gameField(field), m_graphicsDevice(device), 
	m_outerRect(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::UI),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dNextAreaOut),
	m_innerRect(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::Background),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dNextAreaIn)
{}

void PreviewComponent::Draw() const {
	using UI::MainWindow::Preview::offsets;
	using UI::MainWindow::d2dNextAreaIn;
	using UI::MainWindow::d2dNextAreaOut;

	m_outerRect.Draw();
	m_innerRect.Draw();

	m_graphicsDevice.Context()->DrawTextW(L"Next:", 5, m_graphicsDevice.TextFormat(),
		D2D1::RectF(d2dNextAreaOut.rect.left, d2dNextAreaOut.rect.top,
			d2dNextAreaOut.rect.right, d2dNextAreaIn.rect.top),
		m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::Text));

	const Tetramino& next = m_gameField.GetNextTetramino();
	ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(next.GetType());
	for (const auto& block : next.GetTetramino()) {
		auto it = offsets.find(next.GetType());
		float offX = it->second.x;
		float offY = it->second.y;

		vec2 pos = next.GetPos() + block;
		D2D1_RECT_F dst = D2D1::RectF(
			d2dNextAreaIn.rect.left + offX + pos.x * blockSize,
			d2dNextAreaIn.rect.top + offY + pos.y * blockSize,
			d2dNextAreaIn.rect.left + offX + (pos.x + 1) * blockSize,
			d2dNextAreaIn.rect.top + offY + (pos.y + 1) * blockSize
		);

		m_graphicsDevice.Context()->DrawBitmap(bitmap, dst);
	}
}

ScoreComponent::ScoreComponent(const GraphicsDevice& device, const GameField& field) :
	m_graphicsDevice(device), m_gameField(field), m_rectComponent(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::UI),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dScoreRect) 
{}

void ScoreComponent::Draw() const {
	m_rectComponent.Draw();
	std::wstring text = L"Score: " + std::to_wstring(m_gameField.GetScore());
	m_graphicsDevice.Context()->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		m_graphicsDevice.TextFormat(), UI::MainWindow::d2dScoreRect.rect,
		m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::Text));
}

HighScoreComponent::HighScoreComponent(const GraphicsDevice& device, const GameField& field) :
	m_graphicsDevice(device), m_gameField(field), m_rectComponent(device.Context(),
		device.GetBrush(GraphicsDevice::BrushType::UI),
		device.GetBrush(GraphicsDevice::BrushType::Border),
		UI::MainWindow::d2dHighRect) 
{}

void HighScoreComponent::Draw() const {
	m_rectComponent.Draw();
	std::wstring text = L"High: " + std::to_wstring(m_gameField.GetHighScore());
	m_graphicsDevice.Context()->DrawTextW(text.c_str(), static_cast<uint32_t>(text.size()),
		m_graphicsDevice.TextFormat(), UI::MainWindow::d2dHighRect.rect,
		m_graphicsDevice.GetBrush(GraphicsDevice::BrushType::Text));
}

ButtonComponent::ButtonComponent(Button* button) : m_button(button) {}

void ButtonComponent::Draw() const {
	m_button->Draw();
}