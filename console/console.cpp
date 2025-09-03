#include "console.h"
#include <io.h>
#include <stdio.h>
#include <sstream>

namespace console
{
bool Console::IsTTY()
{
#ifdef _WIN32
    return _isatty(_fileno(stdout));
#else
    return isatty(STDOUT_FILENO);
#endif
}

Console::Console()
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(handle, &mode);
    mode &= ~ENABLE_MOUSE_INPUT;
    SetConsoleMode(handle, ENABLE_PROCESSED_INPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

Console::~Console()
{
}

void Console::readline(uint32_t wait, uint32_t timeout)
{
    ss_.str("");
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    //DWORD oldMode = 0;
    //GetConsoleMode(handle, &oldMode);
    //// disable mouse and window input
    //DWORD mode = oldMode ^ ENABLE_MOUSE_INPUT ^ ENABLE_WINDOW_INPUT;
    //SetConsoleMode(handle, mode);
    //FlushConsoleInputBuffer(handle);

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
                    } else if(0 != records[i].Event.KeyEvent.uChar.UnicodeChar) {
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
}

std::string Console::getline()
{
    ss_.str("");
    char buffer[4];
        for(;;){
            if(NULL == fgets(buffer, sizeof(buffer)-1, stdin)){
                break;
            }
            buffer[sizeof(buffer)-1] = '\0';
            char* p;
            p = strchr(buffer, '\r');
            if(nullptr != p){
                *p = '\0';
            }
            p = strchr(buffer, '\n');
            if(nullptr != p){
                *p = '\0';
                ss_ << buffer;
                break;
            }
			ss_ << buffer;
        }
        return ss_.str();
}

int32_t Console::toUtf8(char utf8[8], WCHAR wc)
{
    return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wc, 1, utf8, 6, nullptr, nullptr);
}

Console& operator<<(Console& console, const Console::Endl&)
{
    fputc('\n', stdout);
    return console;
}

Console& operator<<(Console& console, const Console::Flush&)
{
    fflush(stdout);
    return console;
}

Console& operator<<(Console& console, const char* str)
{
    fputs(str, stdout);
    return console;
}

Console& operator<<(Console& console, const std::string& str)
{
    fputs(str.c_str(), stdout);
    return console;
}

Console& operator<<(Console& console, const Console::SGR& sgr)
{
    fputs(sgr.code_, stdout);
    return console;
}
} // namespace console
