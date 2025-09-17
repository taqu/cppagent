#ifndef INC_CPPAGENT_APP_H_
#define INC_CPPAGENT_APP_H_
#include "cppagent.h"
#include "array.h"

namespace cppagent
{
class Agent;
class App
{
public:
    App();
    ~App();
    int32_t run(int32_t argc, char* argv[]);

private:
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    Array<Agent*> agents_;
};
} // namespace cppagent
#endif // INC_CPPAGENT_APP_H_
