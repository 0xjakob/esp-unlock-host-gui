#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <future>
#include <algorithm>

#include "command_reader.hpp"

using namespace std;

size_t find_terminator(std::vector<char> needle, std::list<char> haystack) {
    if (!needle.size() || !haystack.size()) return 0;
    size_t needle_it = 0;
    
    for (auto it = haystack.begin(); it != haystack.end(); it++) {
//    for (size_t i = 0; i < haystack.size(); i++) {
        if (*it == needle[needle_it]) {
            needle_it++;
            if (needle_it == needle.size()) {
                return 1;
            }
        } else {
            needle_it = 0;
        }
    }

    return 0;
}

CommandReader::CommandReader(const string& device) : buffer() {
    struct termios serial_port_settings;
    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

    if (fd == -1) {
        throw ReaderException(device, errno);
    }

    int retval = tcgetattr(fd, &serial_port_settings);
	if (retval < 0) {
        throw ReaderException(device, errno);
	}

    serial_port_settings.c_lflag &= ~(ICANON);
    serial_port_settings.c_lflag &= ~(ECHO | ECHOE);
	retval = tcsetattr(fd, TCSANOW, &serial_port_settings);
	if (retval < 0) {
		perror("Failed to set serial attributes");
        throw ReaderException(device, errno);
	}
}

CommandReader::~CommandReader() {
    close(fd);
}

void CommandReader::execute_cmd(std::shared_ptr<SerialCommand> cmd) {
    const std::array<std::vector<char>, 3> DELIMITERS = {T_NL, T_CR, T_ERROR};
    bool reading = true;
    std::vector<char> raw_command = cmd->get_cmd();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    fsync(fd);
    tcflush(fd, TCIOFLUSH);

    size_t write_count = 0;
    while (raw_command.size() > 0) {
        int result = write(fd, raw_command.data(), raw_command.size());
        if (result < 0) {
            throw ReaderException("writing to device failed", errno);
        }

        raw_command.erase(raw_command.begin(), raw_command.begin() + result);
        write_count++;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    fsync(fd);
    tcflush(fd, TCOFLUSH);

    std::list<char> result;
    auto found_terminator = result.end();
    while (found_terminator == result.end()) {
        int chars_read = read(fd, buffer.data(), buffer.size());
        for (int i = 0; i < chars_read; i++) {
            result.push_back(buffer[i]);
        }

        for (const std::vector<char>& delimiter: DELIMITERS) {
            found_terminator = std::find_first_of(result.begin(), result.end(), delimiter.begin(), delimiter.end());
            if (found_terminator != result.end()) {
                break;
            }
        }
    }

    // TODO: it's still possible that another delimiter is present in result before found_terminator
    result.erase(found_terminator, result.end());

//    printf("result:");
//    for (char c: result) {
//        printf("%02X", c);
//    }
//    printf(";\n");

    cmd->parse_answer(result);
    buffer.fill('\0');

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    fsync(fd);
    tcflush(fd, TCIOFLUSH);
}

