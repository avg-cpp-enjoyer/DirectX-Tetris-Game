#include "GameField.hpp"

GameField::GameField() {
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
    const vec2& originalPos = m_currentTetramino.GetPos();
    m_currentTetramino.Move(dir);

    if (!TetraminoFits(m_currentTetramino)) {
        m_currentTetramino.SetPos(originalPos);
        return false;
    }

    return true;
}

bool GameField::RotateCurrent() {
    m_currentTetramino.Rotate();

    const vec2& originalPos = m_currentTetramino.GetPos();
    static const vec2 offsets[7] = {
        vec2(0.0f, 0.0f), 
        vec2(-1.0f, 0.0f), 
        vec2(1.0f, 0.0f),
        vec2(-2.0f, 0.0f), 
        vec2(2.0f, 0.0f), 
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 2.0f) 
    };

    for (const auto& offset : offsets) {
        m_currentTetramino.SetPos(originalPos + offset);
        if (TetraminoFits(m_currentTetramino)) {
            m_ghostTetramino.Rotate();
            return true;
        }
    }

    m_currentTetramino.SetPos(originalPos);
    m_currentTetramino.UndoRotation();

    return false;
}

void GameField::Reset(){
    std::memset(m_grid, 0, sizeof(m_grid));

    m_score = 0;
    m_gameOver = false;

    m_nextTetramino = TetraminoManager::CreateRandomTetramino();
    SpawnNewTetramino();
}

bool GameField::IsGhostCollide() const {
    const vec2& ghostPos = m_ghostTetramino.GetPos();
    auto& ghostBlocks = m_ghostTetramino.GetTetramino();

    __m128 ghostsX = _mm_set_ps(
        ghostPos.x + ghostBlocks[3].x,
        ghostPos.x + ghostBlocks[2].x,
        ghostPos.x + ghostBlocks[1].x,
        ghostPos.x + ghostBlocks[0].x
    );

    __m128 ghostsY = _mm_set_ps(
        ghostPos.y + ghostBlocks[3].y,
        ghostPos.y + ghostBlocks[2].y,
        ghostPos.y + ghostBlocks[1].y,
        ghostPos.y + ghostBlocks[0].y
    );

    const vec2& currentPos = m_currentTetramino.GetPos();
    auto& currentBlocks = m_currentTetramino.GetTetramino();

    for (int i = 0; i < 4; i++) {
        float cx = currentPos.x + currentBlocks[i].x;
        float cy = currentPos.y + currentBlocks[i].y;

        __m128 curX = _mm_set_ps1(cx);
        __m128 curY = _mm_set_ps1(cy);
        __m128 cmpX = _mm_cmpeq_ps(curX, ghostsX);
        __m128 cmpY = _mm_cmpeq_ps(curY, ghostsY);
        __m128 both = _mm_and_ps(cmpX, cmpY);

        if (_mm_movemask_ps(both)) {
            return true;
        }
    }
    return false;
}

bool GameField::TetraminoFits(const Tetramino& tetramino) const {
    const auto& blocks = tetramino.GetTetramino();
    float px = tetramino.GetPos().x;
    float py = tetramino.GetPos().y;

    __m128 offX = _mm_set_ps(blocks[3].x, blocks[2].x, blocks[1].x, blocks[0].x);
    __m128 offY = _mm_set_ps(blocks[3].y, blocks[2].y, blocks[1].y, blocks[0].y);

    __m128 baseX  = _mm_set_ps1(px);
    __m128 baseY  = _mm_set_ps1(py);
    __m128 fx     = _mm_add_ps(baseX, offX);
    __m128 fy     = _mm_add_ps(baseY, offY);

    __m128i ix = _mm_cvttps_epi32(fx);
    __m128i iy = _mm_cvttps_epi32(fy);

    alignas(16) int xs[4]{}, ys[4]{};
    _mm_store_si128((__m128i*)xs, ix);
    _mm_store_si128((__m128i*)ys, iy);

    for (int i = 0; i < 4; i++) {
        unsigned x = static_cast<unsigned>(xs[i]);
        unsigned y = static_cast<unsigned>(ys[i]);

        if (x >= gfWidth || y >= gfHeight) {
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

        if (x >= 0 && x < gfWidth && y >= 0 && y < gfHeight) {
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

    float xs[4]{}, ys[4]{};
    const auto& blocks = m_ghostTetramino.GetTetramino();
    const vec2& pos = m_ghostTetramino.GetPos();
    for (int i = 0; i < 4; i++) {
        xs[i] = pos.x + blocks[i].x;
        ys[i] = pos.y + blocks[i].y;
    }

    float dropDist[4]{};
    for (int i = 0; i < 4; i++ ) {
        int blockX  = int(xs[i]);
        int startY  = int(ys[i]);
        int targetY = startY;

        while (targetY + 1 < gfHeight && m_grid[targetY + 1][blockX] == 0) {
            targetY++;
        }
        dropDist[i] = static_cast<float>(targetY - startY);
    }

    __m128 dropVec  = _mm_set_ps(dropDist[3], dropDist[2], dropDist[1], dropDist[0]);
    __m128 min12    = _mm_min_ps(dropVec, _mm_shuffle_ps(dropVec, dropVec, _MM_SHUFFLE(2, 3, 0, 1)));
    __m128 minFinal = _mm_min_ps(min12, _mm_shuffle_ps(min12, min12, _MM_SHUFFLE(1, 0, 3, 2)));

    vec2 newPos = pos;
    newPos.y += _mm_cvtss_f32(minFinal);
    m_ghostTetramino.SetPos(newPos);
}

void GameField::ClearLines() {
    int writeHeight = gfHeight - 1;
    int linesCleared = 0;

    for (int y = gfHeight - 1; y >= 0; y--) {
        uint8_t* row = m_grid[y];

        if (IsLineFull(row)) {
            ++linesCleared;
            continue;
        }

        if (y != writeHeight) {
            std::memcpy(m_grid[writeHeight], row, gfStride);
        }
        writeHeight--;
    }

	static constexpr int scoreTable[] = { 0, 100, 300, 500, 800 };
	m_score += scoreTable[linesCleared];
}

bool GameField::IsLineFull(const uint8_t* row) {
    __m128i data = _mm_load_si128((__m128i*)row);
    __m128i zero = _mm_setzero_si128();
    __m128i cmp  = _mm_cmpeq_epi8(data, zero);

    return (_mm_movemask_epi8(cmp) & ((1 << gfWidth) - 1)) == 0;
}

void GameField::SpawnNewTetramino() {
    m_currentTetramino = m_nextTetramino;
    m_ghostTetramino = TetraminoManager::CreateTetramino(m_currentTetramino.GetType());
    m_nextTetramino = TetraminoManager::CreateRandomTetramino();
    UpdateGhostPos();
}
