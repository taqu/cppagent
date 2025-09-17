#ifndef INC_CPPAGENT_LOG_H_
#define INC_CPPAGENT_LOG_H_
#include <quill/Logger.h>
#include <quill/LogMacros.h>

namespace cppagent
{
class Log
{
public:
    static void initialize();
    static void terminate();
    static Log& getInstance();

    quill::Logger* Logger()
    {
        return logger_;
    }
private:
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log() = default;
    ~Log() = default;

    static Log instance_;
    quill::Logger* logger_ = nullptr;
};

#define LOG_INFO(fmt, ...) QUILL_LOG_INFO(cppagent::Log::getInstance().Logger(), fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) QUILL_LOG_WARNING(cppagent::Log::getInstance().Logger(), fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) QUILL_LOG_ERROR(cppagent::Log::getInstance().Logger(), fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(cppagent::Log::getInstance().Logger(), fmt, ##__VA_ARGS__)
} // namespace cppagent
#endif // INC_CPPAGENT_LOG_H_
