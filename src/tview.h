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
private:
    ftxui::Element top_;
    std::vector<ftxui::Element> elements_;
};
} // namespace view
#endif //INC_TVIEW_H_
