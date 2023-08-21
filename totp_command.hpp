#pragma once

#include <string>
#include "serial_command.hpp"

class TOTP_Command : public SerialCommand, public AsyncTransfer<std::string> {
public:
    TOTP_Command(std::string otp_name);

    std::vector<char> get_cmd() override;

    void parse_answer(std::list<char> answer) override;

    std::string name;
};

