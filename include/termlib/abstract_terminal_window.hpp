#ifndef ABSTRACT_TERMINAL_WINDOW_HPP
#define ABSTRACT_TERMINAL_WINDOW_HPP

#include"terminal_text_style.hpp"
#include"vec2.hpp"

#include<tl/optional.hpp>

#include<memory>
#include<string_view>

class AbstractTerminalWindow {
public:
    virtual ~AbstractTerminalWindow() {};

    virtual void setCursorPosition(Coord2i position) = 0;

    virtual void moveCursor(Vec2i offset) {
        setCursorPosition(getCursorPosition() + offset);
    };

    virtual Coord2i getCursorPosition() const = 0;

    virtual void put(char ch) = 0;

    virtual void put(std::string_view str) {
        for (char ch : str)
            put(ch);
    }

    virtual void display() = 0;

    virtual tl::optional<char> getChar(int timeoutMillis = -1) = 0;

    virtual void setTextStyle(TextStyle style) = 0;

    virtual void setEchoing(bool echo) = 0;

    bool getEchoing() const {
        return echoing;
    }

    virtual Size2i getSize() const = 0;

    virtual void clear(Color background = Color::Black) = 0;

protected:
    bool echoing = true;
};

std::unique_ptr<AbstractTerminalWindow> createDefaultWindow();

#endif // ABSTRACT_TERMINAL_WINDOW_HPP

