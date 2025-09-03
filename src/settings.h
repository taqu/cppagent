#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_
#include <cstdint>

namespace cpptoml
{
    struct TomlProxy;
}

namespace cppagent
{
class Settings
{
public:
    Settings();
    ~Settings();

    bool load(const char* filepath);
    void setTemperature(float temperature);
    void setTopK(int32_t topK);
    void setTopP(float topP);
private:
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    void traverse_object(cpptoml::TomlProxy& proxy);
    void traverse_console(cpptoml::TomlProxy& proxy);
    int32_t duration_;
    int32_t max_tokens_; //=512;
    float temperature_; //=0.0f;
    int32_t topK_; //=10;
    float topP_; //=1.0f;
};
}
#endif //INC_SETTINGS_H_
