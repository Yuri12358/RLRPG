#ifndef WINAPI_TERMINAL_WINDOW_HPP
#define WINAPI_TERMINAL_WINDOW_HPP

#include"abstract_terminal_window.hpp"

#include"compat/functional.hpp"
#include"compat/type_traits.hpp"

#include<windows.h>
#include<cstdio>
#include<cassert>
#include<chrono>

class WinAPITerminalWindow : public AbstractTerminalWindow {
	using Clock = std::chrono::steady_clock;
	using TimeoutInterval = std::chrono::milliseconds;

public:
    WinAPITerminalWindow()
		: inputHandle(GetStdHandle(STD_INPUT_HANDLE))
		, outputHandle(GetStdHandle(STD_OUTPUT_HANDLE)) {
		WinAPITerminalWindow::setEchoing(false);
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
		return lastKeyPressData.map_or_else(
			bind_front(&WinAPITerminalWindow::extractRepeatedKey, this),
			bind_front(&WinAPITerminalWindow::readKeyFromConsole, this, timeoutAsMillis(timeoutMillis)));
    }

    void setTextStyle(TextStyle style) override {
    }

    void setEchoing(bool echoing) override {
		DWORD consoleModeFlags;
		GetConsoleMode(inputHandle, &consoleModeFlags);
		setBits(consoleModeFlags, ENABLE_ECHO_INPUT, echoing);
		SetConsoleMode(inputHandle, consoleModeFlags);
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
			COORD{ 0, static_cast<SHORT>(-bufferInfo.dwSize.Y)},
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

	[[nodiscard]]
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

	[[nodiscard]]
	static WORD toWinAPIBackgroundColor(Color color) noexcept {
		return toWinAPIForegroundColor(color) << 4;
	}

	tl::optional<char> readKeyFromConsole(tl::optional<TimeoutInterval> timeoutMillis) noexcept {
		namespace chrono = std::chrono;
		Clock::time_point now = Clock::now();
		Clock::time_point const waitUntil = timeoutMillis.map_or(add(now), (Clock::time_point::max)());

		do {
			auto const timeoutLeft = chrono::duration_cast<TimeoutInterval>(waitUntil - now);
			if (WaitForSingleObject(inputHandle, timeoutLeft.count()) != WAIT_OBJECT_0) {
				return tl::nullopt;
			}

			INPUT_RECORD record;
			DWORD read{};
			ReadConsoleInput(inputHandle, &record, 1, &read);
			if (read == 1) {
				switch (record.EventType) {
				case KEY_EVENT:
					processKeyEvent(record.Event.KeyEvent);
					return extractRepeatedKey(*lastKeyPressData);
				default:
					break;
				}
			}

			now = Clock::now();
		} while (now < waitUntil);

		return tl::nullopt;
	}

	struct LastKeyPressData {
		char key{};
		int repeats{};
	};

	char extractRepeatedKey(LastKeyPressData& pressData) noexcept {
		auto const key = pressData.key;
		--pressData.repeats;
		if (pressData.repeats == 0) {
			lastKeyPressData.reset();
		}
		return key;
	}

	void processKeyEvent(KEY_EVENT_RECORD const& event) {
		assert(!lastKeyPressData.has_value());
		lastKeyPressData = LastKeyPressData{ event.uChar.AsciiChar, event.wRepeatCount };
	}

	static tl::optional<TimeoutInterval> timeoutAsMillis(int millis) noexcept {
		if (millis == -1) {
			return tl::nullopt;
		}
		return TimeoutInterval(millis);
	}

	template<class T>
	static void setBits(T& flags, type_identity_t<T> bitMask, bool val) noexcept {
		if (val) {
			flags |= bitMask;
		} else {
			flags &= ~bitMask;
		}
	}

	tl::optional<LastKeyPressData> lastKeyPressData;
	HANDLE inputHandle;
	HANDLE outputHandle;
};

#endif // WINAPI_TERMINAL_WINDOW_HPP

