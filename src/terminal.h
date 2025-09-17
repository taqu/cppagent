#ifndef INC_CPPAGENT_TERMINAL_H_
#define INC_CPPAGENT_TERMINAL_H_
#include <cstdint>
#include <sstream>
#include <string>

#if defined(_WIN32)
#    include <Windows.h>
#    include <conio.h>
#endif

#if defined(__unix__) || defined(__linux__)
#    include <sys/ioctl.h>
#    include <termios.h>
#    include <unistd.h>
#endif

#include <replxx.hxx>

namespace cppagent
{
struct Vector
{
    int32_t x_;
    int32_t y_;
};

enum class Erase : uint8_t
    {
        End = 0, // clear from cursor to the end
        Begin = 1, // clear from cursor to beginning
        Entire = 2, // clear entire
        EntireBuffer = 3, // clear entire and delete all lines in scrollback buffer, only for Erase in Display
    };

class Terminal
{
public:
    inline static constexpr size_t BufferSize = 16;

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

    struct C216BG
    {
        uint8_t r_;
        uint8_t g_;
        uint8_t b_;
    };

    struct Gray
    {
        uint8_t n_;
    };

    struct GrayBG
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
        int32_t n_;
    };

    struct CursorDown
    {
        int32_t n_;
    };

    struct CursorForward
    {
        int32_t n_;
    };

    struct CursorBack
    {
        int32_t n_;
    };

    struct CursorNextLine
    {
        int32_t n_;
    };

    struct CursorPreviousLine
    {
        int32_t n_;
    };

    struct CursorPosition
    {
        int32_t n_; // row
        int32_t m_; // column
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
    Terminal();
    ~Terminal();

    std::string readline(Vector pos, int32_t wait=100, int32_t timeout=1000);

    Vector getSize() const;
    Vector getCursorPosition() const;

private:
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    std::stringstream ss_;
    replxx::Replxx rx_;
#ifdef _WIN32
#else
#endif
};

Terminal& operator<<(Terminal& terminal, const Terminal::Endl& code);
Terminal& operator<<(Terminal& terminal, const Terminal::Flush& code);

Terminal& operator<<(Terminal& terminal, const char* str);
Terminal& operator<<(Terminal& terminal, const std::string& str);

Terminal& operator<<(Terminal& terminal, const Terminal::SGR& sgr);
Terminal& operator<<(Terminal& terminal, const Terminal::C216 rgb);
Terminal& operator<<(Terminal& terminal, const Terminal::C216BG rgb);
Terminal& operator<<(Terminal& terminal, const Terminal::Gray gray);
Terminal& operator<<(Terminal& terminal, const Terminal::GrayBG gray);

Terminal& operator<<(Terminal& terminal, const Terminal::CursorUp pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorDown pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorForward pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorBack pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorNextLine pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorPreviousLine pos);
Terminal& operator<<(Terminal& terminal, const Terminal::CursorPosition pos);
Terminal& operator<<(Terminal& terminal, const Terminal::EraseDisplay pos);
Terminal& operator<<(Terminal& terminal, const Terminal::EraseLine pos);

Terminal& operator<<(Terminal& terminal, int8_t x);
Terminal& operator<<(Terminal& terminal, int16_t x);
Terminal& operator<<(Terminal& terminal, int32_t x);
Terminal& operator<<(Terminal& terminal, int64_t x);
Terminal& operator<<(Terminal& terminal, uint8_t x);
Terminal& operator<<(Terminal& terminal, uint16_t x);
Terminal& operator<<(Terminal& terminal, uint32_t x);
Terminal& operator<<(Terminal& terminal, uint64_t x);
Terminal& operator<<(Terminal& terminal, float x);
Terminal& operator<<(Terminal& terminal, double x);
} // namespace cppagent
#endif // INC_CPPAGENT_TERMINAL_H_
