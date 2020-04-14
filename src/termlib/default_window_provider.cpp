#include<termlib/default_window_provider.hpp>

#ifdef WIN32
#include<termlib/winapi_terminal_window.hpp>
using DefaultTerminalWindow = WinAPITerminalWindow;
#else
#include<termlib/ncurses_terminal_window.hpp>
using DefaultTerminalWindow = NcursesTerminalWindow;
#endif

AbstractTerminalWindow & DefaultWindowProvider::getWindow() {
    static DefaultTerminalWindow window;
    return window;
}

