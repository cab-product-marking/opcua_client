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

#include "logger.h"
#include "os_wrappers.h"
#include "client_open62541.h"
#include "common_types.h"

#include "job_open62541.h"
#include "job_dec_jobtype.h"
#include "job_dec_nodetype.h"

#define TESTING 

namespace cab
{

    #define MAX_JOB_LENGTH      100

    /* Class Client */

    class Client //: private open62541::Client
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
        add_monitored_item(open62541::jsptr);

        void
        delete_monitored_item(open62541::jsptr);

        void 
        read_node(open62541::jsptr);

        void
        write_node(open62541::jsptr);

        void
        browse_nodes(open62541::jsptr);

        void 
        print_label(open62541::jsptr);

        void 
        replace_label(open62541::jsptr);

    private:

        void
        show_usage(void);

        bool
        file_finder(const std::string& dir, const std::string& file);

        void
        create_job(const std::string& input);

        open62541::jsptr
        node_type(const open62541::jsptr, const std::string&, const std::string&);

        std::map<int, std::string>
        parse_args(const std::string& input);

        bool 
        digits(const std::string& str);

        void
        print_jobs_(void);

        void
        print_job(open62541::jsptr);

        /* Variables */

        open62541::Client* client_;

        std::set<open62541::jsptr> jobs_;


    };

} // namespace cab

/* Eof */