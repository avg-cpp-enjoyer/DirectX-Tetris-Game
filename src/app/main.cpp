#include "TetrisWindow.hpp"

int _stdcall wWinMain([[maybe_unused]] _In_ HINSTANCE instance, _In_opt_ HINSTANCE, [[maybe_unused]] _In_ wchar_t* cmdLine, _In_ int cmdShow) {
	TetrisWindow tetris;
	if (!tetris.Create(nullptr, WS_POPUP | WS_VISIBLE, WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, CW_USEDEFAULT, CW_USEDEFAULT,
		UI::MainWindow::mwWidth, UI::MainWindow::mwHeight)) {
		return 1;
	}

	tetris.Exec(cmdShow);
}