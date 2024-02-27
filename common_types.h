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

    /************************************************************************/

    /* Used data types */

    enum class data_type : int
    {
        datetime = 0,
        boolean,
        int16,
        uint16,
        int32,
        uint32,
        int64,
        uint64,
        f32,
        f64,
        string,
        DEFAULT
    };

    /************************************************************************/

    class StringType
    {
    public:

        static std::string 
        type(open62541::data_type type)
        {
            std::string str = "";
            switch(type)
            {
                case data_type::datetime: str = "datetime";
                case data_type::boolean: str = "boolean";
                case data_type::int16: str = "int16";
                case data_type::uint16: str = "uint16";
                case data_type::int32: str = "int32";
                case data_type::uint32: str = "uint32";
                case data_type::int64: str = "int64";
                case data_type::uint64: str = "uint64";
                case data_type::f32: str = "f32";
                case data_type::f64: str = "f64";
                case data_type::string: str = "string";
                case data_type::DEFAULT: str = "DEFAULT";
            }
            return str;
        }

        // static std::string
        // type(open62541::...)

    };

    /************************************************************************/

} // namespace open62541

namespace freeopcua
{

} // namespace freeopcua

/* Eof */