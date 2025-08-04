#include "IComponent.hpp"
#include "model/GameField.hpp"
#include "engine/ResourceManager.hpp"

class TetraminoComponent : public Component {
public:
	TetraminoComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};

class GhostComponent : public Component {
public:
	GhostComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};

class BlockGridComponent : public Component {
public:
	BlockGridComponent(const GraphicsDevice& device, const GameField& field);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};
