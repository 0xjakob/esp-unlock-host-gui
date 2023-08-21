#pragma once

#include <future>
#include <list>
#include <vector>
#include <string>

#define TERMINATOR '\r'

class SerialCommand {
public:
    virtual std::vector<char> get_cmd() = 0;

    virtual void parse_answer(std::list<char> answer) = 0;
    
//    virtual std::future<std::string> get_result() = 0;

//    virtual size_t parse_next(std::vector<char> answer_next) = 0;
    
//    virtual std::vector<char> get_start() = 0;
    
//    virtual std::list<std::vector<char> > get_terminal_symbols() = 0;
};

template<typename T>
class AsyncTransfer {
public:
    AsyncTransfer() : result() { }

    std::future<T> get_result() {
        return result.get_future();
    }

protected:
    std::promise<T> result;
};
