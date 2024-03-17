#pragma once

#include <iostream>

/* Features */

#define FEATURE_TESTING
// #define FEATURE_CLIENT_OFF

/* Color features */

#define FONT_BLACK              "\033[0;30m"
#define FONT_RED                "\033[0;31m"
#define FONT_GREEN              "\033[0;32m"
#define FONT_BROWN              "\033[0;33m"
#define FONT_BLUE               "\033[0;34m"
#define FONT_MAGENTA            "\033[0;35m"
#define FONT_CYAN               "\033[0;36m"
#define FONT_LIGHT_GRAY         "\033[0;37m"
#define FONT_DARK_GRAY          "\033[1;30m"
#define FONT_LIGHT_BLUE         "\033[1;34m"
#define FONT_LIGHT_GREEN        "\033[1;32m"
#define FONT_LIGHT_CYAN         "\033[1;36m"
#define FONT_LIGHT_RED          "\033[1;31m"
#define FONT_LIGHT_PURPLE       "\033[1;35m"
#define FONT_YELLOW             "\033[1;33m"
#define FONT_WHITE              "\033[1;37m"
#define FONT_RESET              "\033[0m"

#define BACK_BLACK              "\033[0;40m"
#define BACK_RED                "\033[0;41m"
#define BACK_GREEN              "\033[0;42m"
#define BACK_BROWN              "\033[0;43m"
#define BACK_BLUE               "\033[0;44m"
#define BACK_MAGENTA            "\033[0;45m"
#define BACK_CYAN               "\033[0;46m"
#define BACK_LIGHT_GREY         "\033[0;47m"

/* Cursor features */

#define CONSOLE_LINE(x)         std::cout << std::string(x, '-') << std::endl;
#define CONSOLE_LINE_50         std::string(50, '-')
#define CONSOLE_LINE_60         std::string(60, '-')
#define CONSOLE_LINE_70         std::string(70, '-')
#define CONSOLE_LINE_80         std::string(80, '-')
#define CONSOLE_LINE_90         std::string(90, '-')
#define CONSOLE_LINE_100        std::string(100, '-')

#define CONSOLE_STARS(x)        std::cout << std::string(x, '*') << std::endl;
#define CONSOLE_STARS_50        std::string(50, '*')
#define CONSOLE_STARS_60        std::string(60, '*')
#define CONSOLE_STARS_70        std::string(70, '*')
#define CONSOLE_STARS_80        std::string(80, '*')
#define CONSOLE_STARS_90        std::string(90, '*')
#define CONSOLE_STARS_100       std::string(100, '*')

#define CURSOR(x,y)             "\033[" << y << ";" << x << "H"
#define CURSOR_COL(x)           "\033[" << x << "G"
#define CURSOR_ROW(y)           "\033[" << y << "d"
#define CURSOR_OFF              "\033[?25l"
#define CURSOR_ON               "\033[?25h"

namespace open62541
{
    #define DEFAULT_IP                  "192.168.200.70"
    #define DEFAULT_PORT                "4840"
    #define DEFAULT_USER                "opcuser"
    #define DEFAULT_PASS                "opcpass"
    #define DEFAULT_URL                 "DEFAULT"
    #define DEFAULT_SECURE_MODE         false
} // namespace open62541

namespace freeopcua
{

} // namespace freeopcua

/* Eof */