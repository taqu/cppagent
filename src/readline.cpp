#include "readline.h"
#include <cassert>
#include "uconv.h"
#include <Windows.h>

namespace cppagent
{
namespace
{
    void add(std::basic_stringstream<char8_t>& ss, const char16_t utf16)
    {
        static constexpr size_t BufferSize = 255;
        char8_t buffer[BufferSize+1];
        size_t length = uconv::utf16_to_utf8(BufferSize, buffer, 1, &utf16);
        buffer[length] = u'\0';
        ss << buffer;
    }
}

Cancellation::~Cancellation()
{
}

std::shared_ptr<Cancellation> Cancellation::create()
{
    return std::make_shared<Cancellation>();
}

Cancellation::Cancellation()
    : cancelled_(false)
{
}

bool Cancellation::isCancelled() const
{
    return cancelled_;
}

void Cancellation::cancel()
{
    cancelled_ = true;
}

ReadLine::ReadLine()
    : x_(0)
    , y_(0)
    , wait_(100)
    , timeout_(0xFFFF'FFFF'FFFF'FFFFULL)
{
}

ReadLine::~ReadLine()
{
}

std::generator<std::tuple<ReadLineState, std::u16string>> ReadLine::read(Terminal& terminal, std::shared_ptr<Cancellation> cancel)
{
    static const std::u16string empty;
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    uint32_t acc = 0;
    static constexpr DWORD Size = 63;
    INPUT_RECORD records[Size] = {};
    while(acc < timeout_) {
        if(cancel && cancel->isCancelled()) {
            goto READLINE_CANCELED;
        }
        DWORD result = WaitForSingleObject(handle, wait_);
        switch(result) {
        case WAIT_ABANDONED:
            goto READLINE_FAIL;
        case WAIT_OBJECT_0:
            acc = 0;
            break;
        case WAIT_TIMEOUT:
            acc += wait_;
            continue;
        default:
            goto READLINE_FAIL;
        }
        DWORD count = 0;
        if(0 == ReadConsoleInput(handle, records, Size, &count)) {
            continue;
        }
        for(uint32_t i = 0; i < count; ++i) {
            switch(records[i].EventType) {
            case FOCUS_EVENT:
                break;
            case KEY_EVENT: {
                if(records[i].Event.KeyEvent.bKeyDown) {
                    if(records[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
                        if((records[i].Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) == 0) {
                            goto READLINE_SUCCESS;
                        }
                        ss_ << u'\n';
                    } else if(0 != records[i].Event.KeyEvent.uChar.UnicodeChar && iswprint(records[i].Event.KeyEvent.uChar.UnicodeChar)) {
                        ss_ << static_cast<char16_t>(records[i].Event.KeyEvent.uChar.UnicodeChar);
                        // fputwc(records[i].Event.KeyEvent.uChar.UnicodeChar, stdout);
                        // fflush(stdout);
                        // fputs(utf8, stdout);
                        // ss_ << utf8;
                    }
                }
            } break;
            case MENU_EVENT:
                break;
            case MOUSE_EVENT:
                break;
            case WINDOW_BUFFER_SIZE_EVENT:
                break;
            default:
                break;
            }
        } // for(uint32_t i=0
    } // while(acc < timeout_)
READLINE_SUCCESS:
    co_yield std::make_tuple(ReadLineState::Success, ss_.str());
READLINE_FAIL:
    co_yield std::make_tuple(ReadLineState::Fail, empty);
READLINE_CANCELED:
    co_yield std::make_tuple(ReadLineState::Cancel, empty);
}
} // namespace cppagent
