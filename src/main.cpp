#include <stdio.h>
#include <string>
#include "httplib.h"
#include "settings.h"
#include "tool.h"
#include "stringbuilder.h"
#include "openaiprovider.h"
#include "terminal.h"

int main(void)
{
    /*{
        httplib::Client cli("https://yahoo.com");
        cli.set_follow_location(true);
        auto res = cli.Get("/news", {{"Accept-Encoding", "gzip"}});
        printf("%d %s\n", res->status, res->body.c_str());
    }*/
    using namespace cppagent;
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
        for(;;){
            fputs(">", stdout);
            std::string line = terminal.readline();
            terminal << Terminal::CursorPosition{0,position.y_+1} << Terminal::EraseLine{Erase::Entire} << Terminal::flush;
            terminal << line << Terminal::flush;
        }
    }
    return 0;
}
