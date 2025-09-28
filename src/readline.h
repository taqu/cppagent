#ifndef INC_CPPAGENT_READLINE_H_
#define INC_CPPAGENT_READLINE_H_
#include <cstdint>
#include <string>
#include <generator>
#include <ranges>
#include <tuple>
#include <memory>
#include <atomic>
#include <sstream>

namespace cppagent
{
    class Terminal;

enum class ReadLineState
{
    Success = 0,
    Continue,
    Fail,
    Cancel,
};

class Cancellation
{
public:
    Cancellation();
    ~Cancellation();
    static std::shared_ptr<Cancellation> create();
    bool isCancelled() const;
    void cancel();
private:
    Cancellation(const Cancellation&) = delete;
    Cancellation& operator=(const Cancellation&) = delete;
    std::atomic_bool cancelled_;
};

class ReadLine
{
public:
    ReadLine();
    ~ReadLine();

    std::generator<std::tuple<ReadLineState, std::u8string>> read(Terminal& terminal, std::shared_ptr<Cancellation> cancel);
private:
    ReadLine(const ReadLine&) = delete;
    ReadLine& operator=(const ReadLine&) = delete;
    int32_t x_;
    int32_t y_;
    uint32_t wait_;
    uint64_t timeout_;
    std::basic_stringstream<char16_t> ss_;
};
} // namespace cppagent
#endif // INC_CPPAGENT_READLINE_H_
