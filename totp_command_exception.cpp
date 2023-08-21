#include "totp_command_exception.hpp"
    
TOTP_CMD_Exception::TOTP_CMD_Exception(const std::string& msg) : message(msg) { }

const char* TOTP_CMD_Exception::what() const noexcept {
    return message.c_str();
}
