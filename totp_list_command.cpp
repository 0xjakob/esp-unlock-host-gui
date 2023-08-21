#include <array>
#include <exception>
#include <time.h>
#include "totp_list_command.hpp"

using namespace std;

TOTP_ListCommand::TOTP_ListCommand() { }

std::vector<char> TOTP_ListCommand::get_cmd() {
    vector<char> cmd;
    for (char c: "TOTP_LIST:") {
        cmd.push_back(c);
    }
    cmd.push_back('\r');

    return cmd;
}

void TOTP_ListCommand::parse_answer(std::list<char> answer) {
    string answer_str(answer.begin(), answer.end());
    std::list<std::string> key_names;
    auto found = answer_str.find("TOTP_LIST:");
    if (found != string::npos) {
        answer_str = answer_str.replace(0, found + string("TOTP_LIST:").size(), "");
    } else {
        answer_str = "ERROR parsing answer";
        printf("received: %s\n", answer_str.c_str());
        // TODO: throw exception?
    }

    found = answer_str.find(',');
    if (found != 0) {
        while (found != string::npos) {
            key_names.push_back(std::string(answer_str.begin(), answer_str.begin() + found));
            answer_str.replace(0, found + 1, "");
            found = answer_str.find(',');
        }
    }
    result.set_value(key_names);
}
