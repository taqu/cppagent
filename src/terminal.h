#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_
#include <cstdint>
#include <sstream>
#include <string>

#if defined(_WIN32)
        #include <Windows.h>
        #include <conio.h>
#endif

#if defined(__unix__) || defined(__linux__)
        #include <termios.h>
        #include <unistd.h>
        #include <sys/ioctl.h>
#endif

#include <replxx.hxx>

namespace console
{
    struct Vector
{
        int32_t x_;
        int32_t y_;
};

class Console
{
public:
    // Select Graphic Rendition
    struct SGR
    {
        const char* code_;
    };
    inline static constexpr SGR Reset = {"\033[0m"};
    inline static constexpr SGR Bold = {"\033[1m"};
    inline static constexpr SGR Faint = {"\033[2m"};
    inline static constexpr SGR Italic = {"\033[3m"};
    inline static constexpr SGR Underline = {"\033[4m"};
    inline static constexpr SGR SlowBlink = {"\033[5m"};
    inline static constexpr SGR RapidBlink = {"\033[6m"};

    inline static constexpr SGR Black = {"\033[30m"};
    inline static constexpr SGR Red = {"\033[31m"};
    inline static constexpr SGR Green = {"\033[32m"};
    inline static constexpr SGR Yellow = {"\033[33m"};
    inline static constexpr SGR Blue = {"\033[34m"};
    inline static constexpr SGR Magenta = {"\033[35m"};
    inline static constexpr SGR Cyan = {"\033[36m"};
    inline static constexpr SGR White = {"\033[37m"};
    inline static constexpr SGR DefaultFG = {"\033[39m"};

    inline static constexpr SGR BlackBG = {"\033[40m"};
    inline static constexpr SGR RedBG = {"\033[41m"};
    inline static constexpr SGR GreenBG = {"\033[42m"};
    inline static constexpr SGR YellowBG = {"\033[43m"};
    inline static constexpr SGR BlueBG = {"\033[44m"};
    inline static constexpr SGR MagentaBG = {"\033[45m"};
    inline static constexpr SGR CyanBG = {"\033[46m"};
    inline static constexpr SGR WhiteBG = {"\033[47m"};
    inline static constexpr SGR DefaultBG = {"\033[49m"};

    struct C216
    {
        uint8_t r_;
        uint8_t g_;
        uint8_t b_;
    };

    struct Gray
    {
        uint8_t n_;
    };

    struct Endl
    {
    };
    inline static constexpr Endl endl;

    struct Flush
    {
    };
    inline static constexpr Flush flush;

    struct CursorUp
    {
        uint8_t n_;
    };

    struct CursorDown
    {
        uint8_t n_;
    };

    struct CursorForward
    {
        uint8_t n_;
    };

    struct CursorBack
    {
        uint8_t n_;
    };

    struct CursorNextLine
    {
        uint8_t n_;
    };

    struct CursorPreviousLine
    {
        uint8_t n_;
    };

    struct CursorPosition
    {
        uint8_t n_;
        uint8_t m_;
    };

    enum class Erase : uint8_t
    {
        End=0,
        Begin=1,
        Entire=2,
    };

    struct EraseDisplay
    {
        Erase n_;
    };

    struct EraseLine
	{
		Erase n_;
	};

    static bool IsTTY();
    Console();
    ~Console();

    std::string readline(uint32_t wait = 1000, uint32_t timeout = 10000);

    Vector getTerminalSize() const;
    Vector getCursorPosition() const;
private:
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    std::stringstream ss_;
    replxx::Replxx rx_;
#ifdef _WIN32
#else
#endif
};

Console& operator<<(Console& console, const Console::Endl& code);
Console& operator<<(Console& console, const Console::Flush& code);

Console& operator<<(Console& console, const char* str);
Console& operator<<(Console& console, const std::string& str);

Console& operator<<(Console& console, const Console::SGR& sgr);
Console& operator<<(Console& console, const Console::C216 rgb);
Console& operator<<(Console& console, const Console::Gray gray);

Console& operator<<(Console& console, const Console::CursorUp pos);
Console& operator<<(Console& console, const Console::CursorDown pos);
Console& operator<<(Console& console, const Console::CursorForward pos);
Console& operator<<(Console& console, const Console::CursorBack pos);
Console& operator<<(Console& console, const Console::CursorNextLine pos);
Console& operator<<(Console& console, const Console::CursorPreviousLine pos);
Console& operator<<(Console& console, const Console::CursorPosition pos);
Console& operator<<(Console& console, const Console::EraseDisplay pos);
Console& operator<<(Console& console, const Console::EraseLine pos);

} // namespace console
#endif // INC_CONSOLE_H_
