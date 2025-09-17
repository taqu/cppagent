#include "readline.h"
#include <Windows.h>

namespace cppagent
{
    namespace
{
        void toUtf8(char8_t* utf8, uint16_t utf16)
    {
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
    :cancelled_(false)
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
    :x_(0)
    ,y_(0)
    ,wait_(100)
        , timeout_(10000)
    {
}

ReadLine::~ReadLine()
{
}

std::generator<std::tuple<ReadLineState, std::string>> ReadLine::read(Terminal& terminal, std::shared_ptr<Cancellation> cancel)
{
    static const std::string empty;
    std::stringstream ss;
    int32_t n=0;
    while(true){
        if(cancel){
            co_yield std::make_tuple(ReadLineState::Cancel, empty);
        }
        co_yield std::make_tuple(ReadLineState::Continue, empty); // yield and return value
        ++n;
        if(10<=n){
            break;
        }
    }
    ss << "ReadLine finished after " << n << " iterations.";
    co_yield std::make_tuple(ReadLineState::Success, ss.str());

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    uint32_t acc = 0;
    static constexpr DWORD Size = 64;
    INPUT_RECORD records[Size] = {};
    char utf8[8] = {};
    while(acc < timeout_) {
        if(cancel) {
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
                            goto READLINE_SUCCESS;
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
READLINE_SUCCESS:
    co_yield std::make_tuple(ReadLineState::Success, empty);
    co_return;
READLINE_FAIL:
    co_yield std::make_tuple(ReadLineState::Fail, empty);
    co_return;
READLINE_CANCELED:
    co_yield std::make_tuple(ReadLineState::Cancel, empty);
    co_return;
}
} // namespace cppagent
