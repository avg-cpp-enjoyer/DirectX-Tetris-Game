#include "TetrisWindow.hpp"

int _stdcall wWinMain([[maybe_unused]] _In_ HINSTANCE instance, _In_opt_ HINSTANCE, [[maybe_unused]] _In_ wchar_t* cmdLine, _In_ int cmdShow) {
	TetrisWindow tetris;
	tetris.Exec(cmdShow);
}