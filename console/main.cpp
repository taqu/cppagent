#include "console.h"

int main(void)
{
	console::Console console;
	console << console::Console::Red << "hello world" << console::Console::Reset << console::Console::endl;
	console.readline();
   return 0;
}

