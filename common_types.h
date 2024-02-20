#pragma once

#include <iostream>
#include <iomanip>
#include <ctime>
#include <map>
#include <memory>

#include "common_defines.h"

typedef float f32_t;
typedef double f64_t;

namespace open62541
{
    /* Connection arguments */

    class server_arguments
    {
    public:
        std::string server_ip = DEFAULT_IP;
        std::string server_port = DEFAULT_PORT;
        std::string server_username = DEFAULT_USER;
        std::string server_password = DEFAULT_PASS;
        bool server_secure_mode = DEFAULT_SECURE_MODE;
        std::string server_url = DEFAULT_URL;

        friend std::ostream& 
        operator<<(std::ostream& os, const server_arguments& args)
        {
            os << CONSOLE_LINE_50 << "\n"
               << std::setw(20) << std::left << "Server IP: " 
               << args.server_ip << "\n"
               << std::setw(20) << std::left << "Server port: " 
               << args.server_port << "\n"
               << std::setw(20) << std::left << "Server username: " 
               << args.server_username << "\n"
               << std::setw(20) << std::left << "Server password: " 
               << args.server_password << "\n"
               << std::setw(20) << std::left << "Server secure mode: " 
               << (args.server_secure_mode ? "Yes" : "No") << "\n"
               << std::setw(20) << std::left << "Server URL: " 
               << args.server_url << "\n"
               << CONSOLE_LINE_50 << "\n";
            return os;
        }
    };

};

namespace freeopcua
{

};

/* Eof */