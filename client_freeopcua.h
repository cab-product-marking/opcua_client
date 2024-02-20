#pragma once

#include <iostream>

#include "Iclient_opcua.h"
#include "job_freeopcua.h"

namespace freeopcua
{

#define DEFAULT_IP                  "192.168.200.70"
#define DEFAULT_PORT                "4840"
#define DEFAULT_USER                "opcuser"
#define DEFAULT_PASS                "opcpass"

    /**
     * Class Client 
     */
    class Client : public opcuac::Client
    {
    public:
        /* Basics */

        using opcuac::Client::Client;

        Client(const Client&) = delete;

        Client&
        operator=(const Client&) = delete;

        Client(Client&&) = delete;

        Client&
        operator=(Client&&) = delete;

        virtual
        ~Client() = default;

        /* To do ... */

    };

} // namespace open62541

/* Eof */