#ifndef INC_CPPAGENT_AGENT_H_
#define INC_CPPAGENT_AGENT_H_
#include "cppagent.h"
#include <generator>
#include <ranges>
#include "stringbuilder.h"
#include "apiprovider.h"
#include "settings.h"
#include "tool.h"
#include "context.h"

namespace cppagent
{
class Agent
{
public:
    Agent();
    ~Agent();
    void run();

    bool load_settings(const char* filepath);
    bool load_tools(const char* filepath);
private:
    Agent(const Agent&) = delete;
    Agent& operator=(const Agent&) = delete;
    void unescape(std::string& str);
    std::generator<int32_t> generate();
    IAPIProvider* apiProvider_;
    IAPIProvider::ChatCompletionRequest request_;
    StringBuilder<> stringBuilder_;
    Settings settings_;
    Tools tools_;
    Context context_;
};
}
#endif //INC_CPPAGENT_AGENT_H_

