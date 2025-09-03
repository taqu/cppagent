#ifndef INC_CPPAGENT_APIPROVIDER_H_
#define INC_CPPAGENT_APIPROVIDER_H_
#include <cstdint>
#include <string>
#include "array.h"

namespace cppagent
{
    class Tools;
    class Settings;

class IAPIProvider
{
public:
    struct Message
    {
        Message() {}
        Message(const char* role, const char* content)
            : role_(role)
            , content_(content)
        {
        }
        Message(const std::string& role, const std::string& content)
            : role_(role)
            , content_(content)
        {
        }
        Message(Message& other) = default;
        Message(Message&& other) = default;
        Message& operator=(Message& other) = default;
        Message& operator=(Message&& other) = default;

        std::string role_;
        std::string content_;
    };

    struct ChatCompletionRequest
    {
        ChatCompletionRequest() {}
        ChatCompletionRequest(ChatCompletionRequest& other) = default;
        ChatCompletionRequest(ChatCompletionRequest&& other) = default;
        ChatCompletionRequest& operator=(ChatCompletionRequest& other) = default;
        ChatCompletionRequest& operator=(ChatCompletionRequest&& other) = default;

        Array<Message> messages_;
    };

    struct Usage
    {
        int32_t prompt_tokens_;
        int32_t completion_tokens_;
        int32_t total_tokens_;
    };
    struct ChatCompletionResponse
    {
        ChatCompletionResponse()
            : status_(-1)
            , created_(0)
            , usage_{}
        {
        }
        ChatCompletionResponse(ChatCompletionResponse& other) = default;
        ChatCompletionResponse(ChatCompletionResponse&& other) = default;
        ChatCompletionResponse& operator=(ChatCompletionResponse& other) = default;
        ChatCompletionResponse& operator=(ChatCompletionResponse&& other) = default;

        bool isSuccess() const;

        int32_t status_;
        std::string id_;
        int64_t created_;
        Usage usage_;
        Array<Message> messages_;
    };

    virtual ~IAPIProvider();
    virtual ChatCompletionResponse generate(const ChatCompletionRequest& request, const Tools& tools, const Settings& settings) = 0;
protected:
    IAPIProvider(const IAPIProvider&) = delete;
    IAPIProvider& operator=(const IAPIProvider&) = delete;

    IAPIProvider();
};
} // namespace cppagent
#endif // INC_CPPAGENT_APIPROVIDER_H_
