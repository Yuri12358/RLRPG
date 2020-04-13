#include<termlib/default_window_provider.hpp>

#ifdef WIN32
#include<termlib/winapi_terminal_window.hpp>
AbstractTerminalWindow & DefaultWindowProvider::getWindow() {
    static WinAPITerminalWindow window;
    return window;
}
#else
#include<termlib/ncurses_terminal_window.hpp>
AbstractTerminalWindow & DefaultWindowProvider::getWindow() {
    static NcursesTerminalWindow window;
    return window;
}
#endif


