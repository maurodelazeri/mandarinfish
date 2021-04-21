
#include <iostream>
#include <spdlog/spdlog.h>
#include "src/streaming.h"
#include <date/date.h>

using namespace std;

template<class Precision>
std::string getISOCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return date::format("%FT%T", date::floor<Precision>(now));
}

int main() {
    spdlog::info("Build generated: {} {}", __DATE__, __TIME__);

    cout << getISOCurrentTimestamp<chrono::microseconds>() << endl;

    cout << "\n"
            "███    ███  █████  ███    ██ ██████   █████  ██████  ██ ███    ██ ███████ ██ ███████ ██   ██ \n"
            "████  ████ ██   ██ ████   ██ ██   ██ ██   ██ ██   ██ ██ ████   ██ ██      ██ ██      ██   ██ \n"
            "██ ████ ██ ███████ ██ ██  ██ ██   ██ ███████ ██████  ██ ██ ██  ██ █████   ██ ███████ ███████ \n"
            "██  ██  ██ ██   ██ ██  ██ ██ ██   ██ ██   ██ ██   ██ ██ ██  ██ ██ ██      ██      ██ ██   ██ \n"
            "██      ██ ██   ██ ██   ████ ██████  ██   ██ ██   ██ ██ ██   ████ ██      ██ ███████ ██   ██ \n"
            "                                                                                             \n"
            "                                                                                             " << endl;

    std::unique_ptr<Streaming> stream = std::make_unique<Streaming>();
    stream->start();

    return 0;
}