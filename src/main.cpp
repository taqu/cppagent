#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include "tview.h"

std::string Code(ftxui::Event event)
{
    std::string codes;
    for(auto& it: event.input()) {
        codes += " " + std::to_string((unsigned int)it);
    }
    return codes;
}

int main()
{
    tview::TView tview;

    using namespace ftxui;
    auto screenInteractive = ScreenInteractive::TerminalOutput();

    std::vector<Event> keys;

    auto left_column = Renderer([&] {
        Elements children = {
            text("Codes"),
            separator(),
        };
        for(size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
            children.push_back(text(Code(keys[i])));
        }
        Element e = emptyElement();
        return vbox(children);
    });

    auto right_column = Renderer([&] {
        Elements children = {
            text("Event"),
            separator(),
        };
        for(size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
            children.push_back(text(keys[i].DebugString()));
        }
        return vbox(children);
    });

    int split_size = 40;
    auto component = ResizableSplitLeft(left_column, right_column, &split_size);
    component |= border;

    std::string text;
    Component input_text = Input(&text, "text");
    input_text |= CatchEvent([&](Event event) {
        if(event.is_mouse() || event.is_cursor_position() || event.is_cursor_shape()){
            return false;
        }
        if(Event::Return == event){
            return true;
        }
        keys.push_back(event);
        return false;
    });

    auto container = Container::Vertical({component, input_text});

    //component |= CatchEvent([&](Event event) {
    //    keys.push_back(event);
    //    return false;
    //});

    Loop loop(&screenInteractive, container);
    while(!loop.HasQuitted()) {
        loop.RunOnce();
    }
    return 0;
}
