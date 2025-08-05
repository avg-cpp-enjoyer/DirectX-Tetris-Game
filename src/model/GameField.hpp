#pragma once

#include <immintrin.h>

#include "Tetramino.hpp"
#include "TetraminoManager.hpp"
#include "HighScoreManager.hpp"
#include "ui/Constants.hpp"

using namespace UI::MainWindow::GameField;

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
private:
	void ClearLines();
	bool IsLineFull(const uint8_t* row);
	void SpawnNewTetramino();
private:
	static constexpr int gfStride = 16;
	alignas(gfStride) uint8_t m_grid[gfHeight][gfStride]{};
	Tetramino m_currentTetramino = TetraminoManager::CreateRandomTetramino();
	Tetramino m_nextTetramino = TetraminoManager::CreateRandomTetramino();
	Tetramino m_ghostTetramino = TetraminoManager::CreateTetramino(m_currentTetramino.GetType());
	int m_score = 0;
	bool m_gameOver = false;
};