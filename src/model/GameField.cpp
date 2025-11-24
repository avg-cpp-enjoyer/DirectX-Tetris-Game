#include "GameField.hpp"
#include "HighScoreManager.hpp"
#include "TetraminoTypes.hpp"
#include "Tetramino.hpp"
#include "TetraminoManager.hpp"

#include <cstring>
#include <core/vec2.hpp>
#include <cstdint>

GameField::GameField() : 
	m_currentTetramino(TetraminoManager::CreateRandomTetramino()),
    m_nextTetramino(TetraminoManager::CreateRandomTetramino()),
    m_ghostTetramino(TetraminoManager::CreateTetramino(m_currentTetramino.GetType())) {
	UpdateGhostPos();
}

bool GameField::IsGameOver() const {
	return m_gameOver;
}

int GameField::GetScore() const {
	return m_score;
}

int GameField::GetHighScore() {
	return HighScoreManager::Get();
}

TetraminoType GameField::GetCell(int x, int y) const {
	return static_cast<TetraminoType>(m_grid[y][x]);
}

const Tetramino& GameField::GetCurrentTetramino() const {
	return m_currentTetramino;
}

const Tetramino& GameField::GetGhostTetramino() const {
	return m_ghostTetramino;
}

const Tetramino& GameField::GetNextTetramino() const {
	return m_nextTetramino;
}

Tetramino& GameField::GetCurrentTetramino() {
	return m_currentTetramino;
}

Tetramino& GameField::GetGhostTetramino() {
	return m_ghostTetramino;
}

void GameField::HardDrop() {
	m_currentTetramino.HardDrop(m_ghostTetramino.GetPos());
}

bool GameField::MoveCurrent(Direction dir) {
	const float2 originalPos = m_currentTetramino.GetPos();
	m_currentTetramino.Move(dir);

	if (!TetraminoFits(m_currentTetramino)) {
		m_currentTetramino.SetPos(originalPos);
		return false;
	}

	UpdateGhostPos();
	return true;
}

bool GameField::RotateCurrent() {
	m_currentTetramino.Rotate();
	float2 originalPos = m_currentTetramino.GetPos();

	static const float2 offsets[7] = {
		float2(0.0f,  0.0f),
		float2(-1.0f, 0.0f),
		float2(1.0f,  0.0f),
		float2(-2.0f, 0.0f),
		float2(2.0f,  0.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  2.0f)
	};

	for (const auto& offset : offsets) {
		m_currentTetramino.SetPos(originalPos + offset);
		if (TetraminoFits(m_currentTetramino)) {
			m_ghostTetramino.SetPos(m_currentTetramino.GetPos());
			m_ghostTetramino.Rotate();
			UpdateGhostPos();
			return true;
		}
	}

	m_currentTetramino.SetPos(originalPos);
	m_currentTetramino.UndoRotation();

	return false;
}

void GameField::Reset() {
	m_score = 0;
	m_gameOver = false;

	std::memset(m_grid, 0, sizeof(m_grid));
	m_nextTetramino = TetraminoManager::CreateRandomTetramino();
	SpawnNewTetramino();
}

bool GameField::IsGhostCollide() const {
	const float2& ghostPos = m_ghostTetramino.GetPos();
	const float2& currentPos = m_currentTetramino.GetPos();
	float dy = ghostPos.y - currentPos.y;

	if ((m_currentTetramino.GetType() == TetraminoType::TETRAMINO_I && dy >= 4.0f) || (dy >= 3.0f)) {
		return false;
	}

	const auto& ghostBlocks = m_ghostTetramino.GetTetramino();
	const auto& currentBlocks = m_currentTetramino.GetTetramino();

	for (const auto& currentBlock : currentBlocks) {
		for (const auto& ghostBlock : ghostBlocks) {
			if (currentBlock == ghostBlock) {
				return true;
			}
		}
	}

	return false;
}

bool GameField::TetraminoFits(const Tetramino& tetramino) const {
	const auto& blocks = tetramino.GetTetramino();
	const float2& pos = tetramino.GetPos();

	for (const auto& block : blocks) {
		int x = static_cast<int>(pos.x + block.x);
		int y = static_cast<int>(pos.y + block.y);

		if (x < 0 || x >= width || y < 0 || y >= height) {
			return false;
		}

		if (m_grid[y][x] != 0) {
			return false;
		}
	}
	return true;
}

void GameField::LockTetramino() {
    m_currentTetramino.SetPos(m_ghostTetramino.GetPos());

    for (const auto& block : m_currentTetramino.GetTetramino()) {
        int x = static_cast<int>(m_currentTetramino.GetPos().x + block.x);
        int y = static_cast<int>(m_currentTetramino.GetPos().y + block.y);
        if (x >= 0 && x < width && y >= 0 && y < height) {
            m_grid[y][x] = static_cast<uint8_t>(m_currentTetramino.GetType());
        }
    }

    ClearLines();
    if (!TetraminoFits(m_nextTetramino)) {
        m_gameOver = true;
        HighScoreManager::CheckAndUpdate(m_score);
        return;
    }

    SpawnNewTetramino();
}

void GameField::UpdateGhostPos() {
	m_ghostTetramino = m_currentTetramino;

	const auto& blocks = m_ghostTetramino.GetTetramino();
	const float2& pos = m_ghostTetramino.GetPos();
	float minDrop = static_cast<float>(height);

	for (const auto& block : blocks) {
		int blockX = static_cast<int>(pos.x + block.x);
		int startY = static_cast<int>(pos.y + block.y);
		int targetY = startY;

		while (targetY + 1 < height && m_grid[targetY + 1][blockX] == 0) {
			targetY++;
		}

		float dropDist = static_cast<float>(targetY - startY);
		if (dropDist < minDrop) {
			minDrop = dropDist;
		}
	}

	float2 newPos = pos;
	newPos.y += minDrop;
	m_ghostTetramino.SetPos(newPos);
}

void GameField::ClearLines() {
	int writeHeight = height - 1;
	int linesCleared = 0;

	for (int y = height - 1; y >= 0; y--) {
		bool fullLine = true;
		for (int x = 0; x < width; x++) {
			if (m_grid[y][x] == 0) {
				fullLine = false;
				break;
			}
		}

		if (fullLine) {
			linesCleared++;
			continue;
		}

		if (y != writeHeight) {
			std::memmove(m_grid[writeHeight], m_grid[y], width);
		}

		writeHeight--;
	}

	for (int y = writeHeight; y >= 0; y--) {
		std::memset(m_grid[y], 0, width);
	}

	static constexpr int scoreTable[] = { 0, 100, 300, 500, 800 };
	m_score += scoreTable[linesCleared];
}

void GameField::SpawnNewTetramino() {
    m_currentTetramino = m_nextTetramino;
    m_ghostTetramino = TetraminoManager::CreateTetramino(m_currentTetramino.GetType());
    m_nextTetramino = TetraminoManager::CreateRandomTetramino();
    UpdateGhostPos();
}
