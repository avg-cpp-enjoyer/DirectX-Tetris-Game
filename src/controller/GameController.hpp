#pragma once

#include <model/GameField.hpp>
#include <engine/Scene.hpp>
#include <model/Tetramino.hpp>

#include <mutex>
#include <ratio>
#include <chrono>
#include <thread>
#include <functional>
#include <cstdint>
#include <cstdarg>

class GameController {
public:
	enum class Command { MoveLeft, MoveRight, MoveDown, Rotate, HardDrop, Pause };

	GameController();
	void Start(uint32_t threadPriority, uintptr_t affinityMask);
	void Shutdown();
	std::mutex& GetGfMutex();
	void ExecuteCommand(Command cmd);
	bool PauseGame();
	GameField& GetGameField();
	void SetGameOverCallback(std::function<void()> callback);
	void SetSceneCallback(std::function<Scene&()> callback);
	void StopExecution();
	void Restart();
private:
	void LogicLoop();
	void GravityStep();
	void UpdateAll() const;
private:
	GameField m_gameField;
	Tetramino& m_currentTetramino;
	std::mutex m_gameFieldMutex;
	std::thread m_logicThread;
	std::function<void()> m_GameOverCallback;
	std::function<Scene&()> m_sceneCallback;
	bool m_logicRunning = false;
	bool m_isPaused = false;
	bool m_pendingLock = false;

	std::chrono::time_point<std::chrono::steady_clock, 
		std::chrono::duration<double, std::nano>> m_nextTick;
};