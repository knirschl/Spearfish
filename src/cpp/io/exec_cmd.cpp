#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "exec_cmd.hpp"

int exec(const char *cmd, std::string &output) {
    std::array<char, buffer_size> buffer{};

    auto pipe(popen(cmd, "r"));
    if (!pipe) {
        // Call failed
        output = "Call to popen() failed";
        return 255;
    }
    size_t count;
    while (!feof(pipe)) {
        while ((count = fread(buffer.data(), 1, buffer_size, pipe))) {
            output.insert(output.end(), std::begin(buffer), std::next(std::begin(buffer), count));
        }
    }

    return pclose(pipe);
}

int exec(const std::string &cmd, std::string &output) {
    return exec(cmd.c_str(), output);
}
