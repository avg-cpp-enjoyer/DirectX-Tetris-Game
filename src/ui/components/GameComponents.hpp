#pragma once

#include "Component.hpp"

#include <engine/RenderTarget.hpp>
#include <core/vec2.hpp>
#include <model/GameField.hpp>
#include <model/TetraminoTypes.hpp>
#include <model/Tetramino.hpp>

#include <array>
#include <mutex>

class CurrentTetraminoView : public I2DGraphicsComponent {
public:
	CurrentTetraminoView(const RenderTarget& target, GameField& field, std::mutex& gfMutex);
	void Draw() const override;
	void Update(float dt) override;
private:
	const std::array<float2, 4>& m_blocks;
	const float2& m_visualPos;
	const TetraminoType& m_type;
	std::mutex& m_gfMutex;
	Tetramino& m_tetramino;
};

class GhostTetraminoView : public I2DGraphicsComponent {
public:
	GhostTetraminoView(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
	const Tetramino& m_currentTetramino;
	const std::array<float2, 4>& m_blocks;
	const float2& m_visualPos;
};

class GameFieldView : public I2DGraphicsComponent {
public:
	GameFieldView(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
};
