#pragma once

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <memory>
#include <vector>
#include <array>
#include <list>
#include "serial_command.hpp"

static const size_t BUFFER_SIZE = 64;
static const std::vector<char> T_CR = {'\r'};
static const std::vector<char> T_NL = {'\n'};
static const std::vector<char> T_ERROR = {'E', 'R', 'R', 'O', 'R'};

size_t find_terminator(std::vector<char> needle, std::vector<char> haystack);

class ReaderException : public std::exception {
public:
    ReaderException(std::string msg, int error_num) : msg(msg), error_number(error_num) { }

    virtual const char* what() const throw() {
        return msg.c_str();
    }

    int get_error_number() {
        return error_number;
    }

private:
    std::string msg;

    int error_number;
};

class CommandReader {
public:
    CommandReader(const std::string& device);

    ~CommandReader();

    void execute_cmd(std::shared_ptr<SerialCommand> cmd);

private: 
    std::array<char, BUFFER_SIZE> buffer;

    int fd;
};
