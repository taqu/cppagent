#ifndef INC_CPPAGENT_TOOL_H_
#define INC_CPPAGENT_TOOL_H_
#include <string>
#include <any>
#include "array.h"

namespace cpptoml
{
    struct TomlProxy;
}

namespace cppagent
{
enum class ToolType
{
    Function,
};

enum class ToolPropertyType
{
    String,
    Number,
    Integer,
    Boolean,
    Array,
    Object,
    Invalid,
};

ToolPropertyType get_tool_property_type(const std::string& type);
const char* tool_property_type_to_string(ToolPropertyType type);

enum class ToolPropertySelection
{
    None,
    Enum,
};

enum class ToolParametersType
{
    Object,
};

struct ToolProperty
{
public:
    std::string name_;
    std::string description_;
    ToolPropertyType type_ = ToolPropertyType::Invalid;
    ToolPropertyType typeInArray_ = ToolPropertyType::Invalid;
    ToolPropertySelection selection_type_ = ToolPropertySelection::None;
    Array<std::string> selection_items_;
};

struct ToolParameters
{
public:
    ToolParametersType type_ = ToolParametersType::Object;
    Array<ToolProperty> properties_;
    Array<std::string> required_;
};

struct Tool
{
public:
    ToolType type_ = ToolType::Function;
    std::string name_;
    std::string description_;
    bool strict_ = true;
    ToolParameters parameters_;
};

class Tools
{
public:
    Tools();
    ~Tools();
    void clear();
    void add(const Tool& tool);
    void add(Tool&& tool);
    bool add(const char* filepath);

    uint32_t size() const;
    const Tool& operator[](uint32_t index) const;
private:
    Tools(const Tools&) = delete;
    Tools& operator=(const Tools&) = delete;

    void traverse_object(cpptoml::TomlProxy& proxy);
    void traverse_tools(cpptoml::TomlProxy& proxy);
    bool traverse_tool(Tool& tool, cpptoml::TomlProxy& proxy);
    bool traverse_parameters(Tool& tool, cpptoml::TomlProxy& proxy);
    bool traverse_properties(Tool& tool, cpptoml::TomlProxy& proxy);
    bool traverse_property(ToolProperty& property, cpptoml::TomlProxy& proxy);

    Array<Tool> tools_;
};
} // namespace cppagent
#endif // INC_CPPAGENT_TOOL_H_
