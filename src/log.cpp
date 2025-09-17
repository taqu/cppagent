#include "log.h"
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/FileSink.h>
#include <quill/std/Array.h>

namespace cppagent
{
Log Log::instance_;

void Log::initialize()
{
    // Backend  
  quill::BackendOptions backend_options;
  quill::Backend::start(backend_options);

  // Frontend
  std::shared_ptr<quill::v10::Sink> file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>("agent.log");
  instance_.logger_ = quill::Frontend::create_or_get_logger("root", std::move(file_sink));

#ifdef _DEBUG
    instance_.logger_->set_log_level(quill::LogLevel::Info);
#else
    instance_.logger_->set_log_level(quill::LogLevel::Error);
    #endif
}

void Log::terminate()
{
    quill::Frontend::remove_logger(instance_.logger_);
    instance_.logger_ = nullptr;
}

Log& Log::getInstance()
{
    return instance_;
}

} // namespace cppagent