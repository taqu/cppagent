#include "openaiprovider.h"
#ifdef _DEBUG
#include <iostream>
#endif
#include "cppjson.h"
#include "settings.h"
#include "tool.h"

namespace cppagent
{
OpenAIProvider::OpenAIProvider()
    : client_("https://api.openai.com")
{
}

OpenAIProvider::OpenAIProvider(const std::string& endpoint)
    : endpoint_(endpoint)
    , client_(endpoint)
{
}

OpenAIProvider::OpenAIProvider(const std::string& endpoint, const std::string& token)
    : endpoint_(endpoint)
    , token_(token)
    , client_(endpoint)

{
}

OpenAIProvider::~OpenAIProvider()
{
}

OpenAIProvider::OpenAIProvider(OpenAIProvider&& other)
    : endpoint_(std::move(other.endpoint_))
    , token_(std::move(other.token_))
    , client_(std::move(other.client_))
{
}

OpenAIProvider& OpenAIProvider::operator=(OpenAIProvider&& other)
{
    if(this != &other) {
        endpoint_ = std::move(other.endpoint_);
        token_ = std::move(other.token_);
        client_ = std::move(other.client_);
    }
    return *this;
}

void OpenAIProvider::setToken(const std::string& token)
{
    token_ = token;
}

const std::string OpenAIProvider::path_ = "/chat/completions";
const std::string OpenAIProvider::content_type_ = "application/json";

IAPIProvider::ChatCompletionResponse OpenAIProvider::generate(const ChatCompletionRequest& request, const Tools& tools, const Settings& settings)
{
    headers_.clear();
    if(!token_.empty()) {
        client_.set_bearer_token_auth(token_);
        headers_.insert({"Authorization", "Bearer " + token_});
    }
    headers_.insert({"Accept-Encoding", "gzip"});
    std::string content = generateRequest(request, tools, settings);
#ifdef _DEBUG
    std::cout << content << std::endl;
    #endif

    httplib::Result result = client_.Post(path_, headers_, content, content_type_);

    ChatCompletionResponse response;
    response.status_ = result->status;
    if(result){
        parseResponse(response, result->body);
    }
    return response;
}

std::string OpenAIProvider::generateRequest(const ChatCompletionRequest& request, const Tools& tools, const Settings& settings)
{
    stringBuilder_.clear();
    stringBuilder_.append("{");
    if(!model_.empty()) {
        stringBuilder_.append("\"model\": \"").append(model_).append("\",");
    }

    addMessages(request);

    if(0 < tools.size()) {
        stringBuilder_.append(",");
        addTools(tools);
    }

    stringBuilder_.append("}");
    return stringBuilder_.toString();
}

void OpenAIProvider::addMessages(const ChatCompletionRequest& request)
{
    stringBuilder_.append("\"messages\": [");
    for(uint32_t i = 0; i < request.messages_.size(); ++i) {
        const Message& message = request.messages_[i];
        stringBuilder_.append("{\"role\": \"").append(message.role_).append("\",");
        stringBuilder_.append("\"content\": \"").append(message.content_).append("\"}");
        if(i < (request.messages_.size() - 1)) {
            stringBuilder_.append(",");
        }
    }
    stringBuilder_.append("]");
}

void OpenAIProvider::addTools(const Tools& tools)
{
    stringBuilder_.append("\"tools\": [");
    for(uint32_t i = 0; i < tools.size(); ++i) {
        const Tool& tool = tools[i];
        stringBuilder_.append("{\"type\": \"function\",");
        stringBuilder_.append("\"name\": \"").append(tool.name_).append("\",");
        stringBuilder_.append("\"content\": \"").append(tool.description_).append("\",");
        stringBuilder_.append("\"strict\": ").append(tool.strict_).append(",");

        stringBuilder_.append("\"parameters\": {");
        stringBuilder_.append("{\"type\": \"object\",");

		stringBuilder_.append("\"properties\": {");
		for(uint32_t j = 0; j < tool.parameters_.properties_.size(); ++j) {
			const ToolProperty& property = tool.parameters_.properties_[j];
			stringBuilder_.append("\"").append(property.name_).append("\": {");
			stringBuilder_.append("\"type\": \"").append(tool_property_type_to_string(property.type_)).append("\",");
            stringBuilder_.append("\"description\": \"").append(property.description_).append("\"");
            if(property.selection_type_ == ToolPropertySelection::Enum && 0<property.selection_items_.size()) {
                stringBuilder_.append(",");
                stringBuilder_.append("\"enum\": [");
				for(uint32_t k = 0; k < property.selection_items_.size(); ++k) {
					stringBuilder_.append("\"").append(property.selection_items_[k]).append("\"");
					if(k < (property.selection_items_.size() - 1)) {
						stringBuilder_.append(",");
					}
				}
				stringBuilder_.append("]");
            }
            if(property.typeInArray_ != ToolPropertyType::Invalid){
                stringBuilder_.append(",");
                stringBuilder_.append("\"items\": {").append("\"type\": \"").append(tool_property_type_to_string(property.typeInArray_)).append("\"}");
            }
			stringBuilder_.append("}");
			if(j < (tool.parameters_.properties_.size() - 1)) {
				stringBuilder_.append(",");
			}
		}
		stringBuilder_.append("}");
        if(0 < tool.parameters_.required_.size()) {
            stringBuilder_.append(",");
            stringBuilder_.append("\"required\": [");
            for(uint32_t j = 0; j < tool.parameters_.required_.size(); ++j) {
                const Tool& tool = tools[i];
                stringBuilder_.append("\"").append(tool.parameters_.required_[j]).append("\"");
                if(i < (tool.parameters_.required_.size() - 1)) {
                    stringBuilder_.append(",");
                }
            }
            stringBuilder_.append("]");
        }

        stringBuilder_.append("},");
        stringBuilder_.append("\"strict\": ").append(tool.strict_).append("}");
        if(i < (tools.size() - 1)) {
            stringBuilder_.append(",");
        }
    }
    stringBuilder_.append("]");
}

void OpenAIProvider::parseResponse(ChatCompletionResponse& response, const std::string& body)
{
    using namespace cppjson;
    JsonReader reader;
    bool result = reader.parse(body.c_str(), body.c_str() + body.size());
    if(!result){
        return;
    }
    JsonProxy proxy = reader.root();
    if(proxy.type() != JsonType::Object){
        return;
    }
    for(JsonProxy i=proxy.begin(); i; i=i.next()){
        if(i.compareKey("created")) {
            response.created_ = i.value().getInt64();
        } else if(i.compareKey("id")) {
            response.id_ = i.value().getString();
        } else if(i.compareKey("usage") && i.value().type() == JsonType::Object) {
            JsonProxy usage = i.value();
            for(JsonProxy j=usage.begin(); j; j=j.next()){
                if(j.compareKey("completion_tokens")) {
                    response.usage_.completion_tokens_ = static_cast<int32_t>(j.value().getInt64());
                }else if(j.compareKey("prompt_tokens")) {
                    response.usage_.prompt_tokens_ = static_cast<int32_t>(j.value().getInt64());
                }else if(j.compareKey("total_tokens")) {
                    response.usage_.total_tokens_ = static_cast<int32_t>(j.value().getInt64());
                }
            }
        } else if(i.compareKey("choices")) {
            JsonProxy choices = i.value();
            if(choices.type() != JsonType::Array || choices.size()<=0){
                continue;
            }
            JsonProxy message = choices.begin().value();
            if(!message || message.type() != JsonType::Object){
                continue;
            }
            message = message.findByKey("message").value();
            JsonProxy role = message.findByKey("role");
            JsonProxy content = message.findByKey("content");
            if(role && content) {
                response.messages_.emplace(role.value().getString(), content.value().getString());
            }
        }
    }
}
} // namespace cppagent
