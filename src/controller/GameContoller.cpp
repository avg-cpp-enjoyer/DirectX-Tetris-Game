#include "GameController.hpp"

#include <ui/components/GameComponents.hpp>
#include <ui/components/UIComponents.hpp>
#include <ui/Constants.hpp>
#include <engine/Scene.hpp>
#include <model/Tetramino.hpp>
#include <model/GameField.hpp>

#include <chrono>
#include <thread>
#include <cstdint>
#include <cstdarg>
#include <functional>
#include <utility>
#include <Windows.h>
#include <mutex>

GameController::GameController() : m_currentTetramino(m_gameField.GetCurrentTetramino()) {}

void GameController::Start(uint32_t threadPriority, uintptr_t affinityMask) {
	m_logicRunning = true;
	m_logicThread = std::thread(&GameController::LogicLoop, this);

	SetThreadPriority(m_logicThread.native_handle(), threadPriority);
	SetThreadAffinityMask(m_logicThread.native_handle(), affinityMask);
}

void GameController::Shutdown() {
	m_logicRunning = false;
	if (m_logicThread.joinable()) {
		m_logicThread.join();
	}
}

std::mutex& GameController::GetGfMutex() {
	return m_gameFieldMutex;
}

void GameController::LogicLoop() {
	static constexpr auto tickInterval = std::chrono::duration<double>(0.5);
	auto now = std::chrono::steady_clock::now();
	m_nextTick = now + tickInterval;

	while (m_logicRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		if (m_pendingLock && !m_currentTetramino.IsAnimating()) {
			m_gameField.LockTetramino();
			m_pendingLock = false;
			m_nextTick = std::chrono::steady_clock::now() + std::chrono::duration<double>(0.5);
			UpdateAll();
		}

		if (!m_logicRunning) {
			break;
		}

		if (m_isPaused) {
			m_nextTick = std::chrono::steady_clock::now() + tickInterval;
			continue;
		}

		if (m_gameField.IsGameOver()) {
			m_GameOverCallback();
			m_isPaused = true;
			m_nextTick = std::chrono::steady_clock::now() + tickInterval;
			continue;
		}

		now = std::chrono::steady_clock::now();
		if (now >= m_nextTick) {
			m_nextTick = now + tickInterval;
			GravityStep();
		}
	}
}

void GameController::ExecuteCommand(Command cmd) {
	if (m_gameField.GetCurrentTetramino().IsDropping()) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_gameFieldMutex);
	switch (cmd) {
	case Command::MoveLeft:
		m_gameField.MoveCurrent(Direction::DIRECTION_LEFT);
		break;
	case Command::MoveRight:
		m_gameField.MoveCurrent(Direction::DIRECTION_RIGHT);
		break;
	case Command::MoveDown:
		m_gameField.MoveCurrent(Direction::DIRECTION_DOWN);
		break;
	case Command::Rotate:
		m_gameField.RotateCurrent();
		break;
	case Command::HardDrop:
		m_gameField.HardDrop();
		m_pendingLock = true;
		break;
	case Command::Pause:
		PauseGame();
		break;
	}

	m_gameField.UpdateGhostPos();
	m_sceneCallback().GetById<GhostTetraminoView>(UI::MainWindow::ghostId)->SetRedraw(true);
}

bool GameController::PauseGame() {
	m_isPaused = !m_isPaused;
	return m_isPaused;
}

GameField& GameController::GetGameField() {
	return m_gameField;
}

void GameController::SetGameOverCallback(std::function<void()> callback) {
	m_GameOverCallback = std::move(callback);
}

void GameController::SetSceneCallback(std::function<Scene&()> callback) {
	m_sceneCallback = std::move(callback);
}

void GameController::GravityStep() {
	std::lock_guard<std::mutex> lock(m_gameFieldMutex);
	if (m_gameField.GetCurrentTetramino().IsDropping()) {
		return;
	}

	m_gameField.UpdateGhostPos();
	m_sceneCallback().GetById<GhostTetraminoView>(UI::MainWindow::ghostId)->SetRedraw(true);

	if (!m_gameField.MoveCurrent(Direction::DIRECTION_DOWN)) {
		m_gameField.LockTetramino();
		UpdateAll();
	}
}

void GameController::UpdateAll() const {
	using namespace UI::MainWindow;

	m_sceneCallback().GetById<GameFieldView>(blockGridId)->SetRedraw(true);
	m_sceneCallback().GetById<GhostTetraminoView>(ghostId)->SetRedraw(true);
	m_sceneCallback().GetById<PreviewComponent>(previewId)->SetRedraw(true);
	m_sceneCallback().GetById<ScoreComponent>(scoreId)->SetRedraw(true);
	m_sceneCallback().GetById<HighScoreComponent>(highScoreId)->SetRedraw(true);
}

void GameController::StopExecution() {
	m_isPaused = true;
}

void GameController::Restart() {
	m_gameField.Reset();
	m_isPaused = false;
	m_nextTick = std::chrono::steady_clock::now();
	UpdateAll();
}
