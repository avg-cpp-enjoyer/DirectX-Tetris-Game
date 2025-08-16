#pragma once

#include "Component.hpp"
#include "model/GameField.hpp"
#include "engine/ResourceManager.hpp"

class TetraminoComponent : public Component {
public:
	TetraminoComponent(const RenderTarget& target, const GameField& field);
	void Draw() const;
private:
	const GameField& m_gameField;
};

class GhostComponent : public Component {
public:
	GhostComponent(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
};

class BlockGridComponent : public Component {
public:
	BlockGridComponent(const RenderTarget& target, const GameField& field);
	void Draw() const override;
private:
	const GameField& m_gameField;
};
