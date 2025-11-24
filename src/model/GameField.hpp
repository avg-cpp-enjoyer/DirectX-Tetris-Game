#pragma once

#include <cstdint>

#include "Tetramino.hpp"
#include "TetraminoTypes.hpp"

class GameField {
public:
	GameField();
	~GameField() = default;
	bool IsGameOver() const;
	int GetScore() const;
	static int GetHighScore();
	TetraminoType GetCell(int x, int y) const;
	const Tetramino& GetCurrentTetramino() const;
	const Tetramino& GetGhostTetramino() const;
	const Tetramino& GetNextTetramino() const;
	Tetramino& GetCurrentTetramino();
	Tetramino& GetGhostTetramino();
	void HardDrop();
	bool MoveCurrent(Direction dir);
	bool RotateCurrent();
	void Reset();
	bool IsGhostCollide() const;
	bool TetraminoFits(const Tetramino& tetramino) const;
	void LockTetramino();
	void UpdateGhostPos();
public:
	static constexpr uint8_t width = 10;
	static constexpr uint8_t height = 20;
private:
	void ClearLines();
	void SpawnNewTetramino();
private:
	uint8_t m_grid[height][width]{};
	Tetramino m_currentTetramino;
	Tetramino m_nextTetramino;
	Tetramino m_ghostTetramino;
	int m_score = 0;
	bool m_gameOver = false;
};