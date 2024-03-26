#pragma once 

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <memory>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <filesystem>
#include <set>
#include <list>

#include "logger.h"
#include "os_wrappers.h"
#include "client_open62541.h"
#include "common_types.h"
#include "common_defines.h"
#include "job_open62541_node.h"

#define MAX_JOB_LENGTH      100

namespace cab
{
    /* Class Client */

    class Client
    {
    public:

        /* Basics */
        Client();

        Client(const Client&) = delete;

        Client&
        operator=(const Client&) = delete;

        Client(Client&&) = delete;

        Client&
        operator=(Client&&) = delete;

        virtual
        ~Client();

        int
        connect(int &argc, char *argv[]);

        void 
        disconnect();

        void
        run_iterate();

        /* Cab service / available jobs */

        void
        add_monitored_item(opcuac::jobsptr);

        void
        delete_monitored_item(opcuac::jobsptr);

        void 
        read_node(opcuac::jobsptr);

        void
        write_node(opcuac::jobsptr);

        void
        browse_nodes(opcuac::jobsptr);

        void 
        print_label(opcuac::jobsptr);

        void 
        replace_label(opcuac::jobsptr);

        void
        upload_stream(opcuac::jobsptr);

    private:

        void
        show_usage(void);

        bool
        file_finder(const std::string& dir, const std::string& file);

        void
        create_job(const std::string& input);

        std::map<int, std::string>
        parse_args(const std::string& input);

        bool 
        digits(const std::string& str);

        opcuac::jobsptr
        nodeID_type(opcuac::jobsptr, const std::string&, const std::string&);

        void
        print_jobs_(void);

        void
        print_job(opcuac::jobsptr);

        opcuac::datasptr  
        init_data(std::map<int, std::string>&) const;

        opc_time_t
        system_time(void) const;

        /* Variables */

        open62541::Client* client_;

        std::list<opcuac::jobsptr> jobs_;

    };

} // namespace cab

/* Eof */