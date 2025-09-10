#include "terminal.h"
#include <algorithm>
#include <io.h>
#include <sstream>
#include <stdio.h>

namespace cppagent
{
bool Terminal::IsTTY()
{
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

Terminal::Terminal()
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(handle, &mode);
    mode &= ~ENABLE_MOUSE_INPUT;
    SetConsoleMode(handle, mode | ENABLE_PROCESSED_INPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    rx_.install_window_change_handler();
}

Terminal::~Terminal()
{
}

std::string Terminal::readline(uint32_t wait, uint32_t timeout)
{
    ss_.str("");
#if 0
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    uint64_t acc = 0;
    static constexpr DWORD Size = 64;
    INPUT_RECORD records[Size] = {};
    char utf8[8] = {};
    while(acc < static_cast<uint64_t>(timeout)){
        DWORD result = WaitForSingleObject(handle, wait);
        switch(result){
            case WAIT_ABANDONED:
                return;
            case WAIT_OBJECT_0:
                acc = 0;
                break;
            case WAIT_TIMEOUT:
                acc += wait;
                continue;
            default:
                return;
        }
        DWORD count = 0;
        if(0 == ReadConsoleInput(handle, records, Size, &count)){
            continue;
        }
        for(uint32_t i=0; i<count; ++i){
            switch(records[i].EventType){
            case FOCUS_EVENT:
                break;
            case KEY_EVENT:{
                if(records[i].Event.KeyEvent.bKeyDown){
					if(records[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        if((records[i].Event.KeyEvent.dwControlKeyState&SHIFT_PRESSED) == 0){
                            goto READLINE_END;
                        }
                        ss_ << '\n';
                        break;
                    } else if(0 != records[i].Event.KeyEvent.uChar.UnicodeChar && iswprint(records[i].Event.KeyEvent.uChar.UnicodeChar)) {
                        int32_t len = toUtf8(utf8, records[i].Event.KeyEvent.uChar.UnicodeChar);
                        fputwc(records[i].Event.KeyEvent.uChar.UnicodeChar, stdout);
                        fflush(stdout);
                        utf8[len] = '\0';
                        //fputs(utf8, stdout);
                        ss_ << utf8;
                    }
				}
            }
                break;
            case MENU_EVENT:
                break;
			case MOUSE_EVENT:
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				break;
			default:
				break;
            }
        }
    }
READLINE_END:
    ss_<<"\n";
    //SetConsoleMode(handle, oldMode);
#endif
    const char* cinput = nullptr;
    for(;;) {
        do {
            cinput = rx_.input("");
        } while((cinput == nullptr) && (errno == EAGAIN));
        if(nullptr != cinput) {
            ss_ << cinput;
        }
        break;
    }
    return ss_.str();
}

Vector Terminal::getSize() const
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(nullptr == handle) {
        // fprintf(stderr, "[ERROR] Couldn't handle terminal\n");
        return {0, 0};
    }

    CONSOLE_SCREEN_BUFFER_INFO info;
    if(GetConsoleScreenBufferInfo(handle, &info) == 0) {
        // fprintf(stderr, "[ERROR] Couldn't get terminal info\n");
        return {0, 0};
    }

    return {info.dwSize.X, info.dwSize.Y};
#else
    struct winsize t_size;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &t_size) == -1) {
        // fprintf(stderr, "[ERROR] Couldn't get terminal info\n");
        return {0, 0};
    }
    return {t_size.ws_col, t_size.ws_row};
#endif
}

Vector Terminal::getCursorPosition() const
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(nullptr == handle) {
        // fprintf(stderr, "[ERROR] Couldn't output handle\n");
        return {1, 1};
    }

    CONSOLE_SCREEN_BUFFER_INFO info;
    if(0 == GetConsoleScreenBufferInfo(handle, &info)) {
        // fprintf(stderr, "[ERROR] Couldn't get screen buffer info\n");
        return {1, 1};
    }

    return {info.dwCursorPosition.X+1, info.dwCursorPosition.Y+1};
#else
    struct termios old_attr, new_attr;

    // Backup terminal attributes
    if(tcgetattr(STDIN_FILENO, &new_attr) == -1) {
        // fprintf(stderr, "[ERROR] Couldn't get terminal attributes\n");
        return {0, 0};
    }

    // Disable echo
    old_attr = new_attr;
    old_attr.c_lflag &= ~(ICANON | ECHO);
    if(tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) == -1) {
        // fprintf(stderr, "[ERROR] Couldn't set terminal attributes\n");
        return {0, 0};
    }

    // Get info about cursor
    if(write(STDOUT_FILENO, "\033[6n", 4) != 4) {
        // fprintf(stderr, "[ERROR] Couldn't get cursor information\n");
        return {0, 0};
    }

    // Get ESC[n;mR value
    char buffer[64] = {0};
    int32_t i = 0;
    for(char ch = 0; ch != 'R'; ++i) {
        if(read(STDIN_FILENO, &ch, 1) != 1) {
            // fprintf(stderr, "[ERROR] Couldn't read cursor information");
            return {0, 0};
        }
        buffer[i] = ch;
    }
    int32_t x = 0, y = 0;
    sscanf(buffer, "\033[%d;%dR", &x, &y);

    // Reset attributes
    if(tcsetattr(0, TCSANOW, &new_attr) == -1) {
        // fprintf(stderr, "[ERROR] Couldn't reset terminal attributes\n");
        // exit(1);
    }
    return {x, y};
#endif
}

Terminal& operator<<(Terminal& terminal, const Terminal::Endl&)
{
    fputc('\n', stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::Flush&)
{
    fflush(stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const char* str)
{
    fputs(str, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const std::string& str)
{
    fputs(str.c_str(), stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::SGR& sgr)
{
    fputs(sgr.code_, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::C216 rgb)
{
    char buffer[Terminal::BufferSize] =  {};
    int32_t c = 16 + 36 * rgb.r_ + 6 * rgb.g_ + rgb.b_;
    c = std::clamp(c, 0, 255);
    snprintf(buffer, Terminal::BufferSize, "\033[38;5;%dm", c);
    fputs(buffer, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::C216BG rgb)
{
    char buffer[Terminal::BufferSize] =  {};
    int32_t c = 16 + 36 * rgb.r_ + 6 * rgb.g_ + rgb.b_;
    c = std::clamp(c, 0, 255);
    snprintf(buffer, Terminal::BufferSize, "\033[48;5;%dm", c);
    fputs(buffer, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::Gray gray)
{
    char buffer[Terminal::BufferSize] =  {};
    int32_t c = 232 + gray.n_;
    c = std::clamp(c, 232, 255);
    snprintf(buffer, Terminal::BufferSize, "\033[38;5;%dm", c);
    fputs(buffer, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::GrayBG gray)
{
    char buffer[Terminal::BufferSize] =  {};
    int32_t c = 232 + gray.n_;
    c = std::clamp(c, 232, 255);
    snprintf(buffer, Terminal::BufferSize, "\033[48;5;%dm", c);
    fputs(buffer, stdout);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorUp pos)
{
    fprintf(stdout, "\033[%dA", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorDown pos)
{
    fprintf(stdout, "\033[%dB", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorForward pos)
{
    fprintf(stdout, "\033[%dC", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorBack pos)
{
    fprintf(stdout, "\033[%dD", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorNextLine pos)
{
    fprintf(stdout, "\033[%dE", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorPreviousLine pos)
{
    fprintf(stdout, "\033[%dF", pos.n_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::CursorPosition pos)
{
    fprintf(stdout, "\033[%d;%dH", pos.n_, pos.m_);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::EraseDisplay pos)
{
    fprintf(stdout, "\033[%dJ", static_cast<int32_t>(pos.n_));
    return terminal;
}

Terminal& operator<<(Terminal& terminal, const Terminal::EraseLine pos)
{
    fprintf(stdout, "\033[%dK", static_cast<int32_t>(pos.n_));
    return terminal;
}

Terminal& operator<<(Terminal& terminal, int8_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, int16_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, int32_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, int64_t x)
{
    fprintf(stdout, "%lld", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, uint8_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, uint16_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, uint32_t x)
{
    fprintf(stdout, "%d", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, uint64_t x)
{
    fprintf(stdout, "%lld", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, float x)
{
    fprintf(stdout, "%f", x);
    return terminal;
}

Terminal& operator<<(Terminal& terminal, double x)
{
    fprintf(stdout, "%f", x);
    return terminal;
}
} // namespace cppagent
