#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_
#include <cstdint>
#include <string>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace console
{
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

	inline static constexpr SGR Red = {"\033[31m"};

	struct Endl
    {
    };
	inline static constexpr Endl endl;

	struct Flush
    {
    };
	inline static constexpr Flush flush;

	static bool IsTTY();
	Console();
	~Console();

	void readline(uint32_t wait=1000, uint32_t timeout=10000);
private:
	Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
	friend Console& operator<<(Console&, const Console::Endl&);
	friend Console& operator<<(Console&, const Console::Flush&);
	friend Console& operator<<(Console&, const char*);
	friend Console& operator<<(Console&, const std::string&);
	friend Console& operator<<(Console&, const Console::SGR&);

	std::string getline();
	std::stringstream ss_;
#ifdef _WIN32
	static int32_t toUtf8(char utf8[8], WCHAR wc);
#else
#endif
};

Console& operator<<(Console& console, const Console::Endl& code);
Console& operator<<(Console& console, const Console::Flush& code);

Console& operator<<(Console& console, const char* str);
Console& operator<<(Console& console, const std::string& str);

Console& operator<<(Console& console, const Console::SGR& sgr);
} // namespace console
#endif //INC_CONSOLE_H_
