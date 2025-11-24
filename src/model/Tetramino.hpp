#pragma once

#include "TetraminoTypes.hpp"
#include "core/vec2.hpp"
#include <array>

enum class Direction {
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
};

class Tetramino {
public:
	void Rotate();
	void UndoRotation();
	void Move(Direction dir);
	const std::array<float2, 4>& GetTetramino() const;
	const TetraminoType& GetType() const;
	const float2& GetPos() const;
	void SetPos(const float2& pos);
	bool IsDropping() const;
	void HardDrop(const float2& ghostPos);
	void UpdateAnimation(float deltaTime);
	bool IsAnimating() const;
	const float2& GetVisualPos() const;
protected:
	Tetramino(TetraminoType type, const float2& pos);
protected:
	std::array<float2, 4> m_tetramino;
	std::array<std::array<float2, 4>, 4> m_rotationStates;
	int m_rotationState = 0;
	bool m_isDropping = false;
	TetraminoType m_type;
	float2 m_pos;
	float2 m_visualPos;      
	float2 m_targetPos;    
	float2 m_startPos;     
	float m_moveElapsed = 0.f;
	float m_moveDuration = 0.f;
	float m_animSpeed = 8.0f;
	float m_dropSpeed = 20.0f;
};

#define DEFINE_TETRAMINO(name) class name : public Tetramino { public: name(); };

DEFINE_TETRAMINO(Tetramino_I)
DEFINE_TETRAMINO(Tetramino_J)
DEFINE_TETRAMINO(Tetramino_O)
DEFINE_TETRAMINO(Tetramino_L)
DEFINE_TETRAMINO(Tetramino_S)
DEFINE_TETRAMINO(Tetramino_Z)
DEFINE_TETRAMINO(Tetramino_T)

#undef DEFINE_TETRAMINO