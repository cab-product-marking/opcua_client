#pragma once

#include "Iclient.h"

#define DEFAULT_IP          "192.168.200.70"
#define DEFAULT_PORT        "4840"
#define DEFAULT_USER        "opcuser"
#define DEFAULT_PASS        "opcpass"

namespace freeopcua
{
    /**
     * Class Client 
     * ^^^^^^^^^^^^
     */
    class Client : public opcuac::IClient
    {
    public:
        /* Basics */

        using opcuac::IClient::IClient;

        Client(const Client&) = delete;

        Client&
        operator=(const Client&) = delete;

        Client(Client&&) = delete;

        Client&
        operator=(Client&&) = delete;

        virtual
        ~Client();

        /* To do ... */

    };

} // namespace open62541

/* Eof */