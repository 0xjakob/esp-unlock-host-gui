#include <string>
#include <exception>
#include <time.h>
#include <math.h>
#include <cctype>
#include <algorithm>

extern "C" {
#include <baseencode.h>
}

#include "totp_command_exception.hpp"
#include "totp_add_command.hpp"

using namespace std;

namespace {

void bin_to_hex(const uint8_t* bin, size_t bin_size, char* hex_out) {
    for (size_t i = 0; i < bin_size; i++) {
        sprintf(hex_out + 2 * i, "%02x", bin[i]);
    }
}

}

TOTP_AddCommand::TOTP_AddCommand(const std::string_view& name, const std::string_view& base32_secret) 
        : name(name) { 
    if (name.empty() || base32_secret.empty()) {
        throw TOTP_CMD_Exception("invalid argument");
    }
    
    secret = std::vector<char>(base32_secret.begin(), base32_secret.end());

    std::transform(secret.begin(), secret.end(), secret.begin(), [](unsigned char c) { return std::toupper(c); } );
    size_t dst_size = ceil(secret.size() / 1.6);
    baseencode_error_t error;
    uint8_t* decoded = base32_decode(reinterpret_cast<const char*>(secret.data()), secret.size(), &error);
    if (error != SUCCESS) {
        throw std::exception();
    }
    
    // reuse secret to store the data formatted as hexadecimal string
    secret.resize(2 * dst_size + 1, '\0');
    bin_to_hex(decoded, dst_size, secret.data());
    secret.pop_back(); // remove null terminator from last sprintf

    free(decoded);
}

std::vector<char> TOTP_AddCommand::get_cmd() {
    vector<char> cmd;
    std::string_view header("TOTP_ADD:");
    for (auto it = header.begin(); it != header.end(); ++it) {
        cmd.push_back(*it);
    }

    cmd.insert(cmd.end(), name.begin(), name.end());
    cmd.push_back(',');
    cmd.insert(cmd.end(), secret.begin(), secret.end());
    printf("command:\"%s\"\n", std::string(cmd.data(), cmd.size()).c_str());
    cmd.push_back('\r');


    return cmd;
}

void TOTP_AddCommand::parse_answer(std::list<char> answer) {
    string answer_str;
    for (auto it = answer.begin(); it != answer.end(); ++it) {
        answer_str.push_back(*it);
    }
    auto found = answer_str.find("TOTP:OK");
    if (found != string::npos) {
        printf("successfully added OTP key\n");
        // TODO: check if result is actually a number
    } else {
        printf("received: %s\n", answer_str.c_str());
        // TODO: throw exception?
    }
    result.set_value();
}
