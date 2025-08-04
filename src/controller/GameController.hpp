#pragma once

#include "core/RingBuffer.hpp"
#include "model/GameField.hpp"
#include "engine/Scene.hpp"

#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>

class GameController {
public:
	enum class Command { MoveLeft, MoveRight, MoveDown, Rotate, HardDrop, CommitDrop, Pause };

	void Start();
	void Shutdown();
	std::mutex& GetGfMutex();
	void LogicLoop();
	void ExecuteCommand(Command cmd);
	void PushCommand(Command cmd);
	void ClearCommands();
	bool PauseGame();
	void OnDrop();
	GameField& GetGameField();
	std::atomic<bool>& GetPengingLock();
	void SetGameOverCallback(std::function<void()> callback);
	void SetUISceneCallback(std::function<Scene&()> callback);
	void SetGameSceneCallback(std::function<Scene&()> callback);
	void GravityStep();
	void StopExecution();
	void Restart();
private:
	std::condition_variable    m_cmdCV;
	std::mutex                 m_cmdMutex;
	std::atomic<bool>          m_logicRunning{ false };
	std::atomic<bool>          m_isPaused{ false };
	RingBuffer<Command, 128>   m_commands;
	GameField                  m_gameField;
	std::mutex                 m_gameFieldMutex;
	std::atomic<bool>          m_pendingLock{ false };
	std::thread                m_logicThread;
	std::function<void()>      m_GameOverCallback;
	std::function<Scene&()>    m_UISceneCallback;
	std::function<Scene&()>    m_GameSceneCallback;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> m_nextTick;
};