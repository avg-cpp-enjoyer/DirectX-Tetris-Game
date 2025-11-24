#include "MainWindow.hpp"

#include <Windows.h>
#include <sal.h>

int _stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ wchar_t*, _In_ int cmdShow) {
	MainWindow win;
	return win.Exec(cmdShow);
}