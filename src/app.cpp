#include "app.h"
#include <argparse/argparse.hpp>
#include "agent.h"

namespace cppagent
{
App::App()
{
}

App::~App()
{
    for(size_t i = 0; i < agents_.size(); ++i) {
        delete agents_[i];
    }
    agents_.clear();
}

int32_t App::run(int32_t argc, char* argv[])
{
    return 0;
}
} // namespace cppagent
