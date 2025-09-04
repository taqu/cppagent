#include "console.h"

int main(void)
{
    console::Console console;
    //console << console::Console::Red << "hello world" << console::Console::Reset << console::Console::endl;
    for(;;) {
        std::string line = console.readline();
        if("/quit" == line) {
            break;
        }
        console << line << console::Console::endl;
    }
    return 0;
}
