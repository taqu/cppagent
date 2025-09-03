#include "settings.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _MSC_VER
#else
#    include <unistd.h>
#endif
#include "cppagent.h"
#include "cpptoml.h"

namespace cppagent
{
Settings::Settings()
    : duration_(0)
    , max_tokens_(512)
    , temperature_(0.0f)
    , topK_(10)
    , topP_(1.0f)
{
}

Settings::~Settings()
{
}

bool Settings::load(const char* filepath)
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

void Settings::setTemperature(float temperature)
{
    temperature_ = temperature;
}

void Settings::setTopK(int32_t topK)
{
    topK_ = topK;
}

void Settings::setTopP(float topP)
{
    topP_ = topP;
}

void Settings::traverse_object(cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("console")) {
            cpptoml::TomlProxy console = i.value();
            traverse_console(console);
        }
    }
}

void Settings::traverse_console(cpptoml::TomlProxy& proxy)
{
    using namespace cpptoml;
    for(TomlProxy i = proxy.begin(); i; i = i.next()) {
        if(i.key().equalsString("duration")) {
            duration_ = static_cast<int32_t>(i.value().getInt64());
        }
    }
}
} // namespace cppagent