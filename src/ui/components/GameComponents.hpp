#include "IComponent.hpp"
#include "model/GameField.hpp"
#include "engine/ResourceManager.hpp"

class TetraminoComponent : public IComponent {
public:
	TetraminoComponent(ID2D1DeviceContext1* context, const GameField& field);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};

class GhostComponent : public IComponent {
public:
	GhostComponent(ID2D1DeviceContext1* context, const GameField& field);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};

class BlockGridComponent : public IComponent {
public:
	BlockGridComponent(ID2D1DeviceContext1* context, const GameField& field);
	void Draw() const override;
private:
	ID2D1DeviceContext1* m_context;
	const GameField& m_gameField;
};
