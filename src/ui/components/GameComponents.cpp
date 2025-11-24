#include "GameComponents.hpp"
#include "Component.hpp"

#include <engine/RenderTarget.hpp>
#include <engine/ResourceManager.hpp>
#include <core/vec2.hpp>
#include <ui/Constants.hpp>
#include <model/Tetramino.hpp>
#include <model/GameField.hpp>
#include <model/TetraminoTypes.hpp>

#include <dcommon.h>
#include <mutex>
#include <d2d1helper.h>
#include <cmath>
#include <d2d1.h>

CurrentTetraminoView::CurrentTetraminoView(const RenderTarget& target, GameField& field, std::mutex& gfMutex) :
	I2DGraphicsComponent(target.Context()),
	m_blocks(field.GetCurrentTetramino().GetTetramino()),
	m_visualPos(field.GetCurrentTetramino().GetVisualPos()),
	m_type(field.GetCurrentTetramino().GetType()),
	m_tetramino(field.GetCurrentTetramino()),
	m_gfMutex(gfMutex)
{}

void CurrentTetraminoView::Draw() const {
	using namespace UI::MainWindow::GameField;
	ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(m_type);

	for (const auto& block : m_blocks) {
		const float2& pos = m_visualPos + block;
		float fx = std::round(offsetX + pos.x * blockSize);
		float fy = std::round(offsetY + pos.y * blockSize);
		m_context->DrawBitmap(bitmap, D2D1::RectF(fx, fy, fx + blockSize, fy + blockSize));
	}
}

void CurrentTetraminoView::Update(float dt) {
	std::lock_guard<std::mutex> lock(m_gfMutex);
	m_tetramino.UpdateAnimation(dt);
	m_needsRedraw = true;
}

GhostTetraminoView::GhostTetraminoView(const RenderTarget& target, const GameField& field) :
	I2DGraphicsComponent(target.Context()), 
	m_gameField(field),
	m_currentTetramino(field.GetCurrentTetramino()),
	m_blocks(field.GetGhostTetramino().GetTetramino()),
	m_visualPos(field.GetGhostTetramino().GetVisualPos())
{}

void GhostTetraminoView::Draw() const {
	using namespace UI::MainWindow::GameField;

	if (!m_gameField.IsGhostCollide() && !m_currentTetramino.IsDropping()) {
		ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(TetraminoType::TETRAMINO_GHOST);

		for (const auto& block : m_blocks) {
			const float2& pos = m_visualPos + block;
			float fx = std::round(offsetX + pos.x * blockSize);
			float fy = std::round(offsetY + pos.y * blockSize);
			m_context->DrawBitmap(bitmap, D2D1::RectF(fx, fy, fx + blockSize, fy + blockSize));
		}
	}
}

GameFieldView::GameFieldView(const RenderTarget& target, const GameField& field)
	: I2DGraphicsComponent(target.Context()), m_gameField(field)
{}

void GameFieldView::Draw() const {
	using namespace UI::MainWindow::GameField;

	for (int y = 0; y < GameField::height; y++) {
		for (int x = 0; x < GameField::width; x++) {
			TetraminoType type = m_gameField.GetCell(x, y);
			if (type != TetraminoType::TETRAMINO_NONE) {
				ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(type);
				float fx = std::round(offsetX + x * blockSize);
				float fy = std::round(offsetY + y * blockSize);
				D2D1_RECT_F dst = D2D1::RectF(fx, fy, fx + blockSize, fy + blockSize);
				m_context->DrawBitmap(bitmap, dst);
			}
		}
	}
}