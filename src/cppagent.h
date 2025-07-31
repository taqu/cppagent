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
    IView(const IView&) = delete;
    IView& operator=(const IView&) = delete;
    IView()
    {
    }
    virtual ~IView()
    {
    }
};

class IAPIProvider
{
public:
    virtual void clear() = 0;
    virtual void add(const std::string& text) = 0;
protected:
    IAPIProvider(const IAPIProvider&) = delete;
    IAPIProvider& operator=(const IAPIProvider&) = delete;

    IAPIProvider()
    {
    }
    virtual ~IAPIProvider()
    {
    }
};

}
#endif //INC_CPPAGENT_H_
