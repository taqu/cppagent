#include <stdio.h>
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "httplib.h"
#include "settings.h"
#include "tool.h"
#include "stringbuilder.h"
#include "openaiprovider.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
    {
        httplib::Client cli("https://yahoo.com");
        cli.set_follow_location(true);
        auto res = cli.Get("/news", {{"Accept-Encoding", "gzip"}});
        printf("%d %s\n", res->status, res->body.c_str());
    }
    using namespace cppagent;
    {
        Settings settings;
        settings.load("../settings.toml");
    }
    {
        StringBuilder<> stringBuilder;
        std::string empty = stringBuilder.toString();
        stringBuilder.print("test%d", 1);
        std::string s = stringBuilder.toString();
        printf("%zd %s\n", s.length(), s.c_str());
    }
    {
        Tools tools;
        tools.add("../default_tools.toml");
    }
    /*{
        OpenAIProvider provider("http://192.168.128.147:9090");
        IAPIProvider::ChatCompletionRequest request;
        request.messages_.push_back({"system", "You are a helpful assistant."});
		request.messages_.push_back({"user", "Hello!!"});
		IAPIProvider::ChatCompletionResponse response = provider.generate(request);
    }*/
    {
    }

#if 0
    using namespace ftxui;
    Component messages;
    std::string prompt;
    auto send_button_callback = [&]{
        if(prompt.empty()){
            return;
        }

        Component message = Renderer([=]{
            return hbox(text(prompt));
        });
        messages->Add(message);
        prompt.clear();
    };

    Component component = Container::Vertical({}); 

    Component input_text = Input(&prompt, "text", InputOption::Default());
    input_text |= CatchEvent([&](Event event) {
        if(event.is_mouse() || event.is_cursor_position() || event.is_cursor_shape()){
            return false;
        }
        if(Event::Return == event){
            SHORT shift = GetKeyState(VK_SHIFT);
            if(shift == 0) {
                send_button_callback();
                input_text->TakeFocus();
                return true;
            }
        }
        return false;
    });

    Component send_button = Button("Send", send_button_callback);
    Component input_composition = Container::Horizontal({input_text, send_button});
    Component input_renderer = Renderer(input_composition, [&]{
        return hbox({input_text->Render(), send_button->Render()});
    });

    std::vector<Component> message_components;
    messages = Container::Vertical(message_components);

    Component main_composition = Container::Vertical({
        messages,
        input_renderer,
    });
    Component main_renderer = Renderer(main_composition, [&] {
        return vbox({
            messages->Render() | vscroll_indicator | frame | yflex | border,
            input_renderer->Render() | border,
        });
    });

    ScreenInteractive screenInteractive = ScreenInteractive::Fullscreen();
    screenInteractive.Loop(main_renderer);
    //Loop loop(&screenInteractive, main_renderer);
    //while(!loop.HasQuitted()) {
    //    loop.RunOnce();
    //}
    #endif
    return 0;
}
