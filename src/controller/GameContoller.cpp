#include "GameController.hpp"

void GameController::Start() {
	m_logicRunning.store(true, std::memory_order_relaxed);
	m_logicThread = std::thread(&GameController::LogicLoop, this);

	SetThreadPriority(m_logicThread.native_handle(), THREAD_PRIORITY_NORMAL);
	SetThreadAffinityMask(m_logicThread.native_handle(), 1ULL << 4);
}

void GameController::Shutdown() {
	m_logicRunning.store(false, std::memory_order_relaxed);
	m_cmdCV.notify_all();

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

	std::unique_lock<std::mutex> lock(m_cmdMutex);
	while (m_logicRunning.load(std::memory_order_relaxed)) {
		m_cmdCV.wait_until(lock, m_nextTick, [&] { return !m_logicRunning.load() || !m_commands.empty(); });

		if (!m_logicRunning.load()) {
			break;
		}

		lock.unlock();
		for (const auto& cmd : m_commands) {
			ExecuteCommand(cmd);
		}

		m_commands.clear();
		lock.lock();

		if (m_isPaused.load(std::memory_order_relaxed)) {
			m_nextTick = std::chrono::steady_clock::now() + tickInterval;
			continue;
		}

		if (m_gameField.IsGameOver()) {
			m_GameOverCallback();
			m_isPaused.store(true);
			m_nextTick = std::chrono::steady_clock::now() + tickInterval;
			continue;
		}

		lock.unlock();
		now = std::chrono::steady_clock::now();
		if (now >= m_nextTick) {
			m_nextTick = now + tickInterval;
			GravityStep();
		}
		lock.lock();
	}
}

void GameController::ExecuteCommand(Command cmd) {
	using UI::MainWindow::ghostId;

	if (m_gameField.GetCurrentTetramino().IsDropping() && cmd != Command::CommitDrop) {
		return;
	}

	std::unique_lock<std::mutex> lock(m_gameFieldMutex);

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
		m_pendingLock.store(true, std::memory_order_relaxed);
		break;
	case Command::Pause:
		PauseGame();
		break;
	}

	m_gameField.UpdateGhostPos();
	m_GameSceneCallback().GetById<GhostComponent>(ghostId)->SetRedraw(true);
}

void GameController::PushCommand(Command cmd) {
	std::lock_guard<std::mutex> lock(m_cmdMutex);
	m_commands.push(cmd);
	m_cmdCV.notify_one();
}

void GameController::ClearCommands() {
	std::lock_guard<std::mutex> lock(m_cmdMutex);
	m_commands.clear();
	m_cmdCV.notify_one();
}

bool GameController::PauseGame() {
	bool newState = !m_isPaused.load();
	m_isPaused.store(newState);
	return newState;
}

void GameController::OnDrop() {
	using namespace UI::MainWindow;

	Tetramino& current = m_gameField.GetCurrentTetramino();
    if (m_pendingLock.load(std::memory_order_relaxed) && !current.IsAnimating()) {
        m_gameField.LockTetramino();
        m_pendingLock.store(false, std::memory_order_relaxed);

        m_GameSceneCallback().GetById<BlockGridComponent>(blockGridId)->SetRedraw(true);
        m_GameSceneCallback().GetById<GhostComponent>(ghostId)->SetRedraw(true);
        m_UISceneCallback().GetById<PreviewComponent>(previewId)->SetRedraw(true);
		m_UISceneCallback().GetById<ScoreComponent>(scoreId)->SetRedraw(true);

		m_nextTick = std::chrono::steady_clock::now() + std::chrono::duration<double>(0.5);
    }
	m_cmdCV.notify_one();
}

GameField& GameController::GetGameField() {
	return m_gameField;
}

std::atomic<bool>& GameController::GetPengingLock() {
	return m_pendingLock;
}

void GameController::SetGameOverCallback(std::function<void()> callback) {
	m_GameOverCallback = std::move(callback);
}

void GameController::SetUISceneCallback(std::function<Scene&()> callback) {
	m_UISceneCallback = std::move(callback);
}

void GameController::SetGameSceneCallback(std::function<Scene&()> callback) {
	m_GameSceneCallback = std::move(callback);
}

void GameController::GravityStep() {
	using namespace UI::MainWindow;

	if (m_gameField.GetCurrentTetramino().IsDropping()) {
		return;
	}

	m_gameField.UpdateGhostPos();
	m_GameSceneCallback().GetById<GhostComponent>(ghostId)->SetRedraw(true);

	if (!m_gameField.MoveCurrent(Direction::DIRECTION_DOWN)) {
		m_gameField.LockTetramino();
		m_GameSceneCallback().GetById<BlockGridComponent>(blockGridId)->SetRedraw(true);
		m_GameSceneCallback().GetById<GhostComponent>(ghostId)->SetRedraw(true);
		m_UISceneCallback().GetById<PreviewComponent>(previewId)->SetRedraw(true);
		m_UISceneCallback().GetById<ScoreComponent>(scoreId)->SetRedraw(true);
	}
}

void GameController::StopExecution() {
	m_isPaused.store(true);
	m_cmdCV.notify_all();
}

void GameController::Restart() {
	using namespace UI::MainWindow;
	{
		std::lock_guard<std::mutex> lock(m_gameFieldMutex);
		m_gameField.Reset();
	}
	m_isPaused.store(false);
	m_cmdCV.notify_all();
	m_nextTick = std::chrono::steady_clock::now();

	m_GameSceneCallback().GetById<BlockGridComponent>(blockGridId)->SetRedraw(true);
	m_GameSceneCallback().GetById<GhostComponent>(ghostId)->SetRedraw(true);
	m_UISceneCallback().GetById<PreviewComponent>(previewId)->SetRedraw(true);
	m_UISceneCallback().GetById<ScoreComponent>(scoreId)->SetRedraw(true);
	m_UISceneCallback().GetById<HighScoreComponent>(highScoreId)->SetRedraw(true);
}
