#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

#include <curl/curl.h>

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
    {
        curl_global_init(CURL_GLOBAL_ALL);
        CURL* curl = curl_easy_init();
        curl_easy_cleanup(curl);
        curl = nullptr;
        curl_global_cleanup();
    }
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
    return 0;
}
