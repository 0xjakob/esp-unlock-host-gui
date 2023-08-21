#pragma once

#include <stdexcept>
#include <string>

class TOTP_CMD_Exception : public std::exception {
public:
    TOTP_CMD_Exception(const std::string& msg);

    const char* what() const noexcept override;

private:
    std::string message;
};
