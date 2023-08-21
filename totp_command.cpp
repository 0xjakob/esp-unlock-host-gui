#include <array>
#include <exception>
#include <time.h>
#include "totp_command_exception.hpp"
#include "totp_command.hpp"

using namespace std;

TOTP_Command::TOTP_Command(std::string otp_name) : name(otp_name) {
    if (name.empty()) {
        throw TOTP_CMD_Exception("invalid argument");
    }
}

std::vector<char> TOTP_Command::get_cmd() {
    vector<char> cmd;
    string header("TOTP:");
    for (auto it = header.begin(); it != header.end(); ++it) {
        cmd.push_back(*it);
    }

    for (auto it = name.begin(); it != name.end(); ++it) {
        cmd.push_back(*it);
    }
    cmd.push_back(',');

    std::array<char, 11> time_string_buf;
    int64_t unix_time = time(nullptr);
    if (unix_time < 0) {
        throw std::exception();
    }
    
    int time_string_size = sprintf(time_string_buf.data(), "%lu", unix_time);
    if (time_string_size < 0) {
        throw std::exception();
    }

    for (auto it = time_string_buf.begin(); it != (time_string_buf.begin() + time_string_size); ++it) {
        cmd.push_back(*it);
    }
    cmd.push_back('\r');

    return cmd;
}

void TOTP_Command::parse_answer(std::list<char> answer) {
    string answer_str;
    for (auto it = answer.begin(); it != answer.end(); ++it) {
        answer_str.push_back(*it);
    }
    auto found = answer_str.find("TOTP:");
    if (found != string::npos) {
        answer_str.replace(0, string("TOTP:").size(), "");
        // TODO: check if result is actually a number
    } else {
        answer_str = "ERROR parsing answer";
        printf("received: %s\n", answer_str.c_str());
        // TODO: throw exception?
    }
    result.set_value(answer_str);
}
