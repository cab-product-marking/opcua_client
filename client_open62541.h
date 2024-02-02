#pragma once

#include "Iclient.h"

#define DEFAULT_IP          "192.168.200.70"
#define DEFAULT_PORT        "4840"
#define DEFAULT_USER        "opcuser"
#define DEFAULT_PASS        "opcpass"

namespace open62541
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

        void
        init(void);

        int
        conntect(const server_arguments&);

        void
        disconnect(void);

        int
        run_iterate(void);

        /* Printer services */

        void
        add_monitored_item(sptr);

        void
        del_monitored_item(sptr);

        void
        read_node(sptr);

        int
        write_node(sptr);

        void
        browse(sptr);

        /* Printer methods */

        void 
        file_upload(sptr);

        void
        print_data(sptr);

        /* Printer interpreter methods */

        void 
        print_current_label(sptr);

    private:

        /**
         * to do
         * add private/internal methods ...
         * write down/copy *.cpp file
        */



    };

} // namespace open62541

/* Eof */