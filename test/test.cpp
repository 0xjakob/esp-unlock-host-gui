#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <string_view>
#include <string>
#include <regex>

#include "totp_add_command.hpp"
#include "totp_command.hpp"

namespace {

const std::vector<char> str_to_vec(const std::string_view& str) {
    std::vector<char> vec(str.begin(), str.end());
    return vec;
}

}

TEST_CASE("TOTP_AddCommand zero size name")
{
    CHECK_THROWS(TOTP_AddCommand("", "base32secret"));
}

TEST_CASE("TOTP_AddCommand zero size secret")
{
    CHECK_THROWS(TOTP_AddCommand("a", ""));
}

TEST_CASE("TOTP_AddCommand Lower case test")
{
    std::string_view test_secret("test2222");
    TOTP_AddCommand cmd("test", test_secret);
    CHECK(cmd.get_cmd() == str_to_vec("TOTP_ADD:test,99253d6b5a\r"));
}

TEST_CASE("TOTP_AddCommand Upper case test")
{
    std::string_view test_secret("TEST2222");
    TOTP_AddCommand cmd("test", test_secret);
    CHECK(cmd.get_cmd() == str_to_vec("TOTP_ADD:test,99253d6b5a\r"));
}

TEST_CASE("TOTP_Command zero size string")
{
    CHECK_THROWS(TOTP_Command(""));
}

TEST_CASE("TOTP_Command minimal input")
{
    TOTP_Command cmd("a");
    const std::regex minimal_cmd_regex("TOTP:a,[0-9]+");
    auto char_cmd = cmd.get_cmd();
    CHECK(std::regex_search(char_cmd.begin(), char_cmd.end(), minimal_cmd_regex));
}
