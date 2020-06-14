#ifndef WINAPI_TERMINAL_WINDOW_HPP
#define WINAPI_TERMINAL_WINDOW_HPP

#include"abstract_terminal_window.hpp"

#include"utils/functional.hpp"
#include"compat/type_traits.hpp"

#include"mapbox/eternal.hpp"
#include"magic_enum/magic_enum.hpp"

#include<windows.h>
#include<cassert>
#include<chrono>

class WinAPITerminalWindow : public AbstractTerminalWindow {
    using Clock = std::chrono::steady_clock;
    using TimeoutInterval = std::chrono::milliseconds;

    constexpr static auto termlib2winAttributes = mapbox::eternal::map<TextStyle::TextAttribute, WORD>({
        { TextStyle::Bold, FOREGROUND_INTENSITY },
        { TextStyle::Underlined, COMMON_LVB_UNDERSCORE }
    });

public:
    WinAPITerminalWindow()
        : inputHandle(::GetStdHandle(STD_INPUT_HANDLE))
        , outputHandle(::GetStdHandle(STD_OUTPUT_HANDLE)) {
        WinAPITerminalWindow::setEchoing(false);
    }

    void setCursorPosition(Coord2i position) override {
        //::SetConsoleCursorPosition(outputHandle, toWinAPI(position));
		cursorPosition = toWinAPI(position);
    }

    [[nodiscard]]
    Coord2i getCursorPosition() const override {
        return fromWin(getConsoleCursorPosition());
    }

    void put(char ch) override {
        //::WriteConsole(outputHandle, &ch, 1, nullptr, nullptr);
        //auto const cursorPosition = getConsoleCursorPosition();
		DWORD charsWritten;
		::WriteConsoleOutputCharacter(outputHandle, &ch, 1, cursorPosition, &charsWritten);
		::WriteConsoleOutputAttribute(outputHandle, &currentAttributes, 1, cursorPosition, &charsWritten);
		++cursorPosition.X;
    }

    void display() override {
    }

    [[nodiscard]]
    tl::optional<char> getChar(int timeoutMillis = -1) override {
        return lastKeyPressData.map_or_else(
            ::bind_front(&WinAPITerminalWindow::extractRepeatedKey, this),
            ::bind_front(&WinAPITerminalWindow::readKeyFromConsole, this, timeoutAsMillis(timeoutMillis)))
            .map(::bind_front(&WinAPITerminalWindow::echoInput, this));
    }

    void setTextStyle(TextStyle style) override {
        //::SetConsoleTextAttribute(outputHandle, toWinAPI(style));
		currentAttributes = toWinAPI(style);
    }

    void setEchoing(bool echoing) override {
        isEchoing = echoing;
    }

    [[nodiscard]]
    Size2i getSize() const override {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        if (::GetConsoleScreenBufferInfo(outputHandle, &bufferInfo)) {
            return Size2i {
                bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1,
                bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1
            };
        }
        assert(false and "Failed to get WinAPI console window size");
        return {};
    }

    void clear(Color background = Color::Black) override {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        ::GetConsoleScreenBufferInfo(outputHandle, &bufferInfo);

        CHAR_INFO charInfo;
        charInfo.Char.AsciiChar = ' ';
        charInfo.Attributes = toWinAPIBackgroundColor(background);

        ::ScrollConsoleScreenBuffer(
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
        /*CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        if (::GetConsoleScreenBufferInfo(outputHandle, &bufferInfo)) {
            return bufferInfo.dwCursorPosition;
        }

        // The function failed. Call GetLastError() for details.
        assert(false and "Failed to get WinAPI console window cursor position");
        return COORD{ 0, 0 };*/
		return cursorPosition;
    }

    [[nodiscard]]
    static COORD toWinAPI(Vec2i coords) noexcept {
        return COORD{
            static_cast<SHORT>(coords.x),
            static_cast<SHORT>(coords.y)
        };
    }

    [[nodiscard]]
    static WORD toWinAPI(TextStyle style) noexcept {
        return toWinAPITextAttributes(style.attributes)
            | toWinAPIForegroundColor(style.getColor().fg)
            | toWinAPIBackgroundColor(style.getColor().bg);
    }

    [[nodiscard]]
    constexpr static WORD toWinAPITextAttributes(int attributes) noexcept {
        WORD result{};
        for (auto const attribute: magic_enum::enum_values<TextStyle::TextAttribute>()) {
            if (attributes & attribute) {
                result |= termlib2winAttributes.at(attribute);
            }
        }
        return result;
    }

    [[nodiscard]]
    static Vec2i fromWin(COORD coords) noexcept {
        return Vec2i{ coords.X, coords.Y };
    }

    [[nodiscard]]
    constexpr static WORD toWinAPIForegroundColor(Color color) noexcept {
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
    constexpr static WORD toWinAPIBackgroundColor(Color color) noexcept {
        return toWinAPIForegroundColor(color) << 4;
    }

    tl::optional<char> readKeyFromConsole(tl::optional<TimeoutInterval> timeoutMillis) noexcept {
        namespace chrono = std::chrono;
        Clock::time_point now = Clock::now();
        Clock::time_point const waitUntil = timeoutMillis.map_or(::add(now), (Clock::time_point::max)());

        do {
            auto const timeoutLeft = chrono::duration_cast<TimeoutInterval>(waitUntil - now);
            if (::WaitForSingleObject(inputHandle, timeoutLeft.count()) != WAIT_OBJECT_0) {
                return tl::nullopt;
            }

            if (auto const key = getPressedKey(); key.has_value()) {
                return key;
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

    void processKeyPress(KEY_EVENT_RECORD const& event) {
        assert(not lastKeyPressData.has_value());
        lastKeyPressData = LastKeyPressData{ event.uChar.AsciiChar, event.wRepeatCount };
    }

    static bool isNormalKeyPress(INPUT_RECORD const& record) noexcept {
        return record.EventType == KEY_EVENT
            and record.Event.KeyEvent.bKeyDown
            and record.Event.KeyEvent.uChar.AsciiChar != '\0';
    }

    tl::optional<char> getPressedKey() {
        INPUT_RECORD record;
        DWORD readCount{};
        ::ReadConsoleInput(inputHandle, &record, 1, &readCount);
        if (readCount == 1 and isNormalKeyPress(record)) {
            processKeyPress(record.Event.KeyEvent);
            return extractRepeatedKey(*lastKeyPressData);
        }
        return tl::nullopt;
    }

    constexpr static tl::optional<TimeoutInterval> timeoutAsMillis(int millis) noexcept {
        if (millis == -1) {
            return tl::nullopt;
        }
        return TimeoutInterval(millis);
    }

    template<class T>
    constexpr static void setBits(T& flags, type_identity_t<T> bitMask, bool val) noexcept {
        if (val) {
            flags |= bitMask;
        } else {
            flags &= ~bitMask;
        }
    }

    char echoInput(char input) noexcept {
        if (isEchoing) {
            put(input);
        }
        return input;
    }

    tl::optional<LastKeyPressData> lastKeyPressData;
	COORD cursorPosition{};
	WORD currentAttributes = toWinAPI(TextStyle{ TerminalColor{ Color::White } });
    HANDLE inputHandle;
    HANDLE outputHandle;
    bool isEchoing = false;
};

#endif // WINAPI_TERMINAL_WINDOW_HPP

