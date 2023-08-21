#pragma once

#include <string_view>
#include "serial_command.hpp"

class TOTP_AddCommand : public SerialCommand, public AsyncTransfer<void> {
public:
    TOTP_AddCommand(const std::string_view& name, const std::string_view& base32_secret);

    std::vector<char> get_cmd() override;

    void parse_answer(std::list<char> answer) override;

    std::string name;

    std::vector<char> secret;
};
