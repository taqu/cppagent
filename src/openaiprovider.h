#ifndef INC_CPPAGENT_OPENAIPROVIDER_H_
#define INC_CPPAGENT_OPENAIPROVIDER_H_
#include "apiprovider.h"
#include "cppagent.h"
#include "httplib.h"
#include "stringbuilder.h"
#include <cstdint>
#include <vector>

namespace cppagent
{
class OpenAIProvider: public IAPIProvider
{
public:
    OpenAIProvider();
    explicit OpenAIProvider(const std::string& endpoint);
    OpenAIProvider(const std::string& endpoint, const std::string& token);
    virtual ~OpenAIProvider();
    OpenAIProvider(OpenAIProvider&& other);
    OpenAIProvider& operator=(OpenAIProvider&& other);

    void setToken(const std::string& token);
    virtual ChatCompletionResponse generate(const ChatCompletionRequest& request, const Tools& tools, const Settings& settings) override;

protected:
    OpenAIProvider(const OpenAIProvider&) = delete;
    OpenAIProvider& operator=(const OpenAIProvider&) = delete;

    std::string generateRequest(const ChatCompletionRequest& request, const Tools& tools, const Settings& settings);
    void addMessages(const ChatCompletionRequest& request);
    void addTools(const Tools& tools);
    void parseResponse(ChatCompletionResponse& response, const std::string& body);

    static const std::string path_;         // = "/v1/completions";
    static const std::string content_type_; // = "application/json";

    std::string endpoint_;
    std::string token_;
    std::string model_;
    httplib::Headers headers_;
    httplib::Client client_;
    StringBuilder<> stringBuilder_;
};

} // namespace cppagent
#endif // INC_CPPAGENT_OPENAIPROVIDER_H_
