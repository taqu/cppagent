#include "agent.h"
#include "openaiprovider.h"
#include "utility.h"

namespace cppagent
{
// Agent
Agent::Agent()
{
    apiProvider_ = CPPA_NEW OpenAIProvider("http://192.168.128.147:9090");
    request_.messages_.add({"system", "You are a helpful assistant."});
}

Agent::~Agent()
{
    CPPA_DELETE(apiProvider_);
}

void Agent::run()
{
    std::string input;
    while(1) {
        std::cin >> input;
        input = strip(input);
        if(input.empty()) {
            continue;
        }
        if(input == "/quit") {
            break;
        }
        request_.messages_.add({"user", input.c_str()});
        IAPIProvider::ChatCompletionResponse response = apiProvider_->generate(request_, tools_, settings_);
        if(response.isSuccess() && 0 < response.messages_.size()) {
            unescape(response.messages_[0].content_);
            std::cout << response.messages_[0].role_ << ": " << response.messages_[0].content_ << std::endl;
        } else {
            std::cout << "No response received from AI" << std::endl;
        }
    }
}

bool Agent::load_settings(const char* filepath)
{
    return settings_.load(filepath);
}

bool Agent::load_tools(const char* filepath)
{
    return tools_.add(filepath);
}

void Agent::unescape(std::string& str)
{
    stringBuilder_.clear();
    for(size_t i = 0; i < str.size(); ++i) {
        if(str[i] == '\\' && (i + 1) < str.size()) {
            if(str[i + 1] == 'n') {
                stringBuilder_.append('\n');
                i += 1;
            } else if(str[i + 1] == 'r') {
                stringBuilder_.append('\r');
                i += 1;
            } else if(str[i + 1] == 't') {
                stringBuilder_.append('\t');
                i += 1;
            } else {
                stringBuilder_.append('\\');
            }
        } else {
            stringBuilder_.append(str[i]);
        }
    }
    str = stringBuilder_.toString();
}
} // namespace cppagent