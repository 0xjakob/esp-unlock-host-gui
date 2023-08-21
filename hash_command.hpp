#pragma once

#include <string>
#include "serial_command.hpp"

class HashCommand : public SerialCommand, public AsyncTransfer<std::string> {
public:
    HashCommand(const std::string &passphrase);

    std::vector<char> get_cmd() override;

    void parse_answer(std::list<char> answer) override;

private:
    std::string passphrase;
};
