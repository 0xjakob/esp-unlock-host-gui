#include "hash_command.hpp"

using namespace std;

HashCommand::HashCommand(const std::string &passphrase) : passphrase(passphrase) { }

std::vector<char> HashCommand::get_cmd() {
    vector<char> cmd;
    string header("PASSPHRASE:");
    for (auto it = header.begin(); it != header.end(); ++it) {
        cmd.push_back(*it);
    }
    for (auto it = passphrase.begin(); it != passphrase.end(); ++it) {
        cmd.push_back(*it);
    }
    cmd.push_back('\r');
    return cmd;
}

void HashCommand::parse_answer(std::list<char> answer) {
    string answer_str;
    for (auto it = answer.begin(); it != answer.end(); ++it) {
        answer_str.push_back(*it);
    }
    auto found = answer_str.find("HASH:");
    if (found != string::npos) {
        answer_str.replace(0, string("Hash:").size(), "");
    }
    result.set_value(answer_str);
}
