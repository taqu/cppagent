#include "tool.h"
#include "cppagent.h"
#include "cpptoml.h"

namespace cppagent
{
namespace
{
    void get_string(std::string& str, cpptoml::TomlProxy proxy)
    {
        uint64_t len = proxy.getStrLen();
        str.resize(len);
        proxy.getString(&str[0]);
    }
} // namespace

ToolPropertyType get_tool_property_type(const std::string& type)
{
    if("string" == type){
        return ToolPropertyType::String;
    }else if("number" == type){
        return ToolPropertyType::Number;
    }else if("integer" == type){
        return ToolPropertyType::Integer;
    }else if("boolean" == type){
        return ToolPropertyType::Boolean;
    }else if("array" == type){
        return ToolPropertyType::Array;
    }
    /* Not support object
    else if("object" == type){
        return ToolPropertyType::Object;
    }
    */
    return ToolPropertyType::Invalid;
}

const char* tool_property_type_to_string(ToolPropertyType type)
{
	switch(type){
		case ToolPropertyType::String:
			return "string";
		case ToolPropertyType::Number:
			return "number";
		case ToolPropertyType::Integer:
			return "integer";
		case ToolPropertyType::Boolean:
			return "boolean";
		case ToolPropertyType::Array:
			return "array";
		default:
			return "";
	}
}

Tools::Tools()
{
}

Tools::~Tools()
{
}

void Tools::clear()
{
    tools_.clear();
}

void Tools::add(const Tool& tool)
{
    tools_.add(tool);
}

void Tools::add(Tool&& tool)
{
    tools_.add(tool);
}

bool Tools::add(const char* filepath)
{
    assert(nullptr != filepath);
    using namespace cpptoml;
    FILE* f = fopen(filepath, "rb");
    if(nullptr == f) {
        return false;
    }
    int64_t size = 0;
#ifdef _MSC_VER
    struct _stat64 s;
    if(0 != _fstat64(fileno(f), &s)) {
        return false;
    }
    size = s.st_size;
#else
    struct stat64 s;
    if(0 != fstat64(fileno(f), &s)) {
        return false;
    }
    size = s.st_size;
#endif
    char* data = (char*)CPPA_MALLOC(static_cast<size_t>(size));
    if(nullptr == data || (0 < size && fread(data, size, 1, f) <= 0)) {
        fclose(f);
        CPPA_FREE(data);
        return false;
    }
    fclose(f);
    TomlParser parser(cppa_malloc, cppa_free);
    bool result = parser.parse(data, data + size);
    assert(result);
    TomlProxy proxy = parser.root();
    traverse_object(proxy);
    CPPA_FREE(data);
    return 0;
}

uint32_t Tools::size() const
{
    return tools_.size();
}

    const Tool& Tools::operator[](uint32_t index) const
{
        return tools_[index];
}

void Tools::traverse_object(cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("tools")) {
            cpptoml::TomlProxy tools = i.value();
            traverse_tools(tools);
        }
    }
}

void Tools::traverse_tools(cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        Tool tool;
        if(!traverse_tool(tool, i)){
            continue;
        }
        tools_.add(std::move(tool));
    }
}

bool Tools::traverse_tool(Tool& tool, cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    std::string type;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("type")) {
            get_string(type, i.value());
        } else if(i.key().equalsString("name")) {
            get_string(tool.name_, i.value());
        } else if(i.key().equalsString("description")) {
            get_string(tool.description_, i.value());
        } else if(i.key().equalsString("strict")) {
            tool.strict_ = i.value().getBool();
        } else if(i.key().equalsString("parameters")) {
            TomlProxy parameters = i.value();
            if(!traverse_parameters(tool, parameters)){
                return false;
            }
        }
    }
    return true;
}

bool Tools::traverse_parameters(Tool& tool, cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    std::string type;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("type")) {
            get_string(type, i.value());
        } else if(i.key().equalsString("properties")) {
            TomlProxy properties = i.value();
            if(!traverse_properties(tool, properties)){
                return false;
            }
        } else if(i.key().equalsString("required")) {
            TomlProxy required = i.value();
            if(required.type() != TomlType::Array) {
                return false;
            }
            for(TomlProxy j = required.begin(); j; j = j.next()) {
                TomlType t = j.type();
                std::string e;
                get_string(e, j);
                tool.parameters_.required_.add(std::move(e));
            }
        }
    }
    return true;
}

bool Tools::traverse_properties(Tool& tool, cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    std::string name;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        ToolProperty property;
        get_string(property.name_, i.key());
        TomlProxy proxy = i.value();
        if(!traverse_property(property, proxy)){
            return false;
        }
        tool.parameters_.properties_.add(std::move(property));
    }
    return true;
}

bool Tools::traverse_property(ToolProperty& property, cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    std::string type;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("type")) {
            get_string(type, i.value());
            property.type_ = get_tool_property_type(type);
            if(property.type_ == ToolPropertyType::Invalid) {
                return false;
            }
        } else if(i.key().equalsString("description")) {
            TomlProxy description = i.value();
            get_string(property.description_, description);
        } else if(i.key().equalsString("enum")) {
            property.selection_type_ = ToolPropertySelection::Enum;
            TomlProxy enums = i.value();
            if(enums.type() != TomlType::Array){
                return false;
            }
            for(TomlProxy j = enums.begin(); j; j = j.next()) {
                std::string e;
                get_string(e, j.value());
                property.selection_items_.add(std::move(e));
            }
        } else if(i.key().equalsString("items")) {
            TomlProxy items = i.value();
            if(items.type() != TomlType::Table){
                return false;
            }
            for(TomlProxy j = items.begin(); j; j = j.next()) {
                if(!j.key().equalsString("type")) {
                    continue;
                }
                get_string(type, j.value());
                property.typeInArray_ = get_tool_property_type(type);
                if(property.typeInArray_ == ToolPropertyType::Invalid || property.typeInArray_ == ToolPropertyType::Array) {
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace cppagent
