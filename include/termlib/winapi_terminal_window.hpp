#ifndef WINAPI_TERMINAL_WINDOW_HPP
#define WINAPI_TERMINAL_WINDOW_HPP

#include"abstract_terminal_window.hpp"
#include<windows.h>

class WinAPITerminalWindow : public AbstractTerminalWindow {
public:
    WinAPITerminalWindow() {
    }

    ~WinAPITerminalWindow() {
    }

    void setCursorPosition(Coord2i position) override {
    }

    Coord2i getCursorPosition() const override {
        Coord2i pos;
        return pos;
    }

    void put(char ch) override {
    }

    void display() override {
    }

    tl::optional<char> getChar(int timeoutMillis = -1) override {
    }

    void setTextStyle(TextStyle style) override {
    }

    void setEchoing(bool e) override {
    }

    Size2i getSize() const override {
    }

    void clear(Color background = Color::Black) override {
    }

private:
};

#endif // WINAPI_TERMINAL_WINDOW_HPP

