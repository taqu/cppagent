#include <stdio.h>
#include <string>
#include <vector>
#include "httplib.h"
#include "settings.h"
#include "tool.h"
#include "stringbuilder.h"
#include "openaiprovider.h"
#include "terminal.h"
#include <replxx.hxx>
#include "log.h"
#include "readline.h"

int main(void)
{
    /*{
        httplib::Client cli("https://yahoo.com");
        cli.set_follow_location(true);
        auto res = cli.Get("/news", {{"Accept-Encoding", "gzip"}});
        printf("%d %s\n", res->status, res->body.c_str());
    }*/
    using namespace cppagent;
    Log::initialize();
    LOG_INFO("test log {}", 1);
    {
        ReadLine readLine;
        bool cancel = false;
        std::generator<std::tuple<ReadLineState, std::string>> gen = readLine.read(cancel);
        int32_t count = 0;
        bool end = false;
        for(auto itr = gen.begin(); itr != gen.end() && !end; ++itr) {
            if(5<=++count){
                cancel = true;
            }
            std::tuple<ReadLineState, std::string> x = *itr;
            switch(std::get<0>(x)) {
            case ReadLineState::Success:
                std::cout << "Success: " << std::get<1>(x) << std::endl;
                end = true;
                break;
            case ReadLineState::Continue:
                std::cout << "Continue" << std::endl;
                break;
            case ReadLineState::Fail:
                std::cout << "Fail" << std::endl;
                end = true;
                break;
            case ReadLineState::Cancel:
                std::cout << "Cancel" << std::endl;
                end = true;
                break;
            }
        }
    }
    {
        Settings settings;
        settings.load("../settings.toml");
    }
    /*{
        StringBuilder<> stringBuilder;
        std::string empty = stringBuilder.toString();
        stringBuilder.print("test%d", 1);
        std::string s = stringBuilder.toString();
        printf("%zd %s\n", s.length(), s.c_str());
    }*/
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
        Terminal terminal;
        Vector size = terminal.getSize();
        Vector position = terminal.getCursorPosition();
        {
            terminal << Terminal::Bold << Terminal::Red << "Bold Red" << Terminal::Reset << Terminal::endl;
            terminal << Terminal::CursorPosition{0,position.y_} << Terminal::EraseLine{Erase::Entire} << Terminal::flush;
        }
        Terminal::CursorPosition inputPosition = {size.y_-1, 0};
        int32_t count = 0;
        std::stringstream ss;
        for(;;){
            ss.str("");
            position = terminal.getCursorPosition();
            terminal << Terminal::CursorPosition{position.y_+1,0};

            //terminal << inputPosition << ">";
            terminal << ">";
            ss << "test" << count;
            std::string line = ss.str();
            std::cout << line << std::endl;
            ++count;
            //terminal << Terminal::CursorPosition{position.y_,0} << Terminal::EraseLine{Erase::Entire};
            //terminal << Terminal::CursorPosition{position.y_+1,0} << Terminal::EraseLine{Erase::Entire} << line << Terminal::flush;
            Sleep(1000);
        }
    }
    Log::terminate();
    return 0;
}
