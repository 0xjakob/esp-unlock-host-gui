#pragma once

#include <list>
#include <string>
#include "serial_command.hpp"

class TOTP_ListCommand : public SerialCommand, public AsyncTransfer<std::list<std::string> > {
public:
    TOTP_ListCommand();

    std::vector<char> get_cmd() override;

    void parse_answer(std::list<char> answer) override;
};
