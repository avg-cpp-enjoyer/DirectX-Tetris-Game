#include "GameComponents.hpp"

TetraminoComponent::TetraminoComponent(const RenderTarget& target, const GameField& field)
	: Component(target.Context()), m_gameField(field)
{}

void TetraminoComponent::Draw() const {
	const Tetramino& current = m_gameField.GetCurrentTetramino();
	ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(current.GetType());
	for (const auto& block : current.GetTetramino()) {
		vec2 pos = current.GetVisualPos() + block;
		float fx = std::round(offsetX + pos.x * blockSize);
		float fy = std::round(offsetY + pos.y * blockSize);
		D2D1_RECT_F dst = D2D1::RectF(fx, fy, fx + blockSize, fy + blockSize);
		m_context->DrawBitmap(bitmap, dst);
	}
}

GhostComponent::GhostComponent(const RenderTarget& target, const GameField& field)
	: Component(target.Context()), m_gameField(field)
{}

void GhostComponent::Draw() const {
	if (!m_gameField.IsGhostCollide() && !m_gameField.GetCurrentTetramino().IsDropping()) {
		const Tetramino& ghost = m_gameField.GetGhostTetramino();
		ID2D1Bitmap* bitmap = ResourceManager::GetTetraminoBitmap(TetraminoType::TETRAMINO_GHOST);
		for (const auto& block : ghost.GetTetramino()) {
			vec2 pos = ghost.GetPos() + block;
			float fx = std::round(offsetX + pos.x * blockSize);
			float fy = std::round(offsetY + pos.y * blockSize);
			D2D1_RECT_F dst = D2D1::RectF(fx, fy, fx + blockSize, fy + blockSize);
			m_context->DrawBitmap(bitmap, dst);
		}
	}
}

BlockGridComponent::BlockGridComponent(const RenderTarget& target, const GameField& field)
	: Component(target.Context()), m_gameField(field)
{}

void BlockGridComponent::Draw() const {
	for (int y = 0; y < gfHeight; y++) {
		for (int x = 0; x < gfWidth; x++) {
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