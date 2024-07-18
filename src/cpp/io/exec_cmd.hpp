//
// Created by fili on 23.08.23.
//

#ifndef SPEARFISH_EXEC_CMD_HPP
#define SPEARFISH_EXEC_CMD_HPP

#include <string>

static const int buffer_size{128};

[[deprecated]]
int exec(const char *cmd, std::string &output);

[[deprecated("Include MAD source as lib")]]
int exec(const std::string &cmd, std::string &output);

#endif // SPEARFISH_EXEC_CMD_HPP
