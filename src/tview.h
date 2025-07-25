#ifndef INC_TVIEW_H_
#define INC_TVIEW_H_
#include <vector>

#include "cppagent.h"
#include "ftxui/dom/elements.hpp"

namespace tview
{
class TView: public cppagent::IView
{
public:
    TView();
    virtual ~TView();

    virtual void clear();
    virtual void add(const std::string& text);

    ftxui::Element get_top();
private:
    std::vector<ftxui::Element> elements_;
};

class RawView: public cppagent::IView
{
public:
    RawView();
    virtual ~RawView();

    virtual void clear();
    virtual void add(const std::string& text);

    ftxui::Element get_top();
private:
    std::vector<ftxui::Element> elements_;
};

} // namespace view
#endif //INC_TVIEW_H_
