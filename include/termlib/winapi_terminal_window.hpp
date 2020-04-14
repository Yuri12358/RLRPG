#ifndef WINAPI_TERMINAL_WINDOW_HPP
#define WINAPI_TERMINAL_WINDOW_HPP

#include"abstract_terminal_window.hpp"
#include<windows.h>
#include<cstdio>
#include<cassert>

class WinAPITerminalWindow : public AbstractTerminalWindow {
public:
    WinAPITerminalWindow()
		: inputHandle(GetStdHandle(STD_INPUT_HANDLE))
		, outputHandle(GetStdHandle(STD_OUTPUT_HANDLE)) {
    }

    void setCursorPosition(Coord2i position) override {
		SetConsoleCursorPosition(outputHandle, toWin(position));
    }

    [[nodiscard]]
	Coord2i getCursorPosition() const override {
		return fromWin(getConsoleCursorPosition());
    }

    void put(char ch) override {
		std::putchar(ch);
    }

    void display() override {
    }

	[[nodiscard]]
    tl::optional<char> getChar(int timeoutMillis = -1) override {
		return tl::nullopt;
    }

    void setTextStyle(TextStyle style) override {
    }

    void setEchoing(bool e) override {
    }

	[[nodiscard]]
    Size2i getSize() const override {
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		if (GetConsoleScreenBufferInfo(outputHandle, &bufferInfo)) {
			return Size2i {
				bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1,
				bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1
			};
		}
		assert(false && "Failed to get WinAPI console window size");
		return {};
    }

    void clear(Color background = Color::Black) override {
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		GetConsoleScreenBufferInfo(outputHandle, &bufferInfo);

		CHAR_INFO charInfo;
		charInfo.Char.AsciiChar = ' ';
		charInfo.Attributes = toWinAPIBackgroundColor(background);

		ScrollConsoleScreenBuffer(
			outputHandle,
			&bufferInfo.srWindow,
			nullptr,
			COORD{ 0, -bufferInfo.dwSize.Y },
			&charInfo
		);
    }

private:
	[[nodiscard]]
	COORD getConsoleCursorPosition() const noexcept {
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		if (GetConsoleScreenBufferInfo(outputHandle, &bufferInfo)) {
			return bufferInfo.dwCursorPosition;
		}

		// The function failed. Call GetLastError() for details.
		assert(false && "Failed to get WinAPI console window cursor position");
		return COORD{ 0, 0 };
	}

	[[nodiscard]]
	static COORD toWin(Vec2i coords) noexcept {
		return COORD{
			static_cast<SHORT>(coords.x),
			static_cast<SHORT>(coords.y)
		};
	}

	[[nodiscard]]
	static Vec2i fromWin(COORD coords) noexcept {
		return Vec2i{ coords.X, coords.Y };
	}

	static WORD toWinAPIForegroundColor(Color color) noexcept {
		switch (color) {
		case Color::Black:   return 0;
		case Color::Red:     return FOREGROUND_RED;
		case Color::Green:   return                  FOREGROUND_GREEN;
		case Color::Blue:    return                                     FOREGROUND_BLUE;
		case Color::Magenta: return FOREGROUND_RED |                    FOREGROUND_BLUE;
		case Color::Yellow:  return FOREGROUND_RED | FOREGROUND_GREEN;
		case Color::Cyan:    return                  FOREGROUND_GREEN | FOREGROUND_BLUE;
		case Color::White:   return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		default:
			assert(false);
			return 0;
		}
	}

	static WORD toWinAPIBackgroundColor(Color color) noexcept {
		return toWinAPIForegroundColor(color) << 4;
	}

	HANDLE inputHandle;
	HANDLE outputHandle;
};

#endif // WINAPI_TERMINAL_WINDOW_HPP

