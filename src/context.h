#ifndef INC_CPPAGENT_CONTEXT_H_
#define INC_CPPAGENT_CONTEXT_H_
#include <cstdint>

namespace cppagent
{
class Context
{
public:
    explicit Context(size_t capacity = 8);
    ~Context();
private:
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
};

} // namespace cppagent
#endif // INC_CPPAGENT_CONTEXT_H_
