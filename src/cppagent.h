#ifndef INC_CPPAGENT_H_
#define INC_CPPAGENT_H_
#include <string>

namespace cppagent
{
class IView
{
public:
    virtual void clear() = 0;
    virtual void add(const std::string& text) = 0;
protected:
    IView()
    {
    }
    virtual ~IView()
    {
    }
};
}
#endif //INC_CPPAGENT_H_
