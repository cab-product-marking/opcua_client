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

#include "defines.h"
#include "logger.h"
#include "local_set.h"

#include "data_types.h"
#include "opcua_client.h"

#define DEFAULT_IP          "192.168.200.70"
#define DEFAULT_PORT        "4840"
#define DEFAULT_USER        "opcuser"
#define DEFAULT_PASS        "opcpass"

#define MAX_JOB_LENGTH      100

#define CAB_USER            FONT_LIGHT_BLUE << "cab" << FONT_RESET

/**
 * Class CAB_Client
 * ^^^^^^^^^^^^^^^^
*/
class CAB_Client : public OPCUA_Client
{
public:

    CAB_Client();

    virtual
    ~CAB_Client();

    CAB_Client(const CAB_Client&) = delete;

    CAB_Client&
    operator=(const CAB_Client&) = delete;

    CAB_Client(CAB_Client&&) = delete;

    CAB_Client&
    operator=(CAB_Client&&) = delete;

    /* Basics */
    int 
    connect(const std::string &url);
    
    int
    connect(int &argc, char *argv[]);

    void 
    disconnect();

    void
    run_iterate();

    void
    job_create(const std::string input, std::shared_ptr<DATA> = nullptr);


    /* Cab service */
    void
    add_monitored_item(std::shared_ptr<JOB>);

    void
    delete_monitored_item(std::shared_ptr<JOB>);

    void 
    read_node(std::shared_ptr<JOB>);

    void
    write_node(std::shared_ptr<JOB>);

    void
    browse_nodes(std::shared_ptr<JOB>);

    void 
    print_label(std::shared_ptr<JOB>);

    void 
    replace_label(std::shared_ptr<JOB>);

    static JOB
    init_job(void);

    static std::shared_ptr<DATA>
    init_data(void);

    /* Info */
    static void 
    show_usage(void);

    
private:

    void
    upload_stream(std::shared_ptr<JOB>);

    bool
    find_file_in_directory(const std::string &directory, 
                        const std::string &file_name);

    std::map<int, std::string> 
    parse_arguments(const std::string& input);

    bool
    convert_string_to_bool(const std::string &string);

    static bool 
    request_all_digits(const std::string &str);

    job_type
    get_job_type(std::string &string);

    job_type
    get_job_type(std::shared_ptr<JOB> job);

    std::string
    get_job_type_string(const job_type &type);

    std::string
    get_id_type_string(const id_type &type);

    std::string
    get_data_type_string(const data_type &type);

    data_type
    get_data_type(const std::string &data_type);

    static int
    int_from_string(std::string &string);

    std::string
    get_url(void);

    void
    set_url(std::string &url);

    void
    set_data_value(const std::string &data_value,
                        std::shared_ptr<JOB> &job);

    std::string
    get_data_value_string(std::shared_ptr<JOB> &job);

    void
    print_job_list(void);

    void
    print_job(std::shared_ptr<JOB> &job);

    /* Include in Logger */
    // void
    // print_console_message(std::shared_ptr<JOB> &job,
    //                     std::string type);
    /* Include in Logger */
    void
    print_console_message_data(std::shared_ptr<JOB> &job,
                        std::string type);

    data_time
    string_data_time(const std::string&);

    data_time 
    get_system_time(void);
    
    bool 
    string_bool(const std::string&);

    int16_t
    string_int16_t(const std::string&);

    uint16_t
    string_uint16_t(const std::string&);

    int32_t 
    string_int32_t(const std::string&);

    uint32_t 
    string_uint32_t(const std::string&);

    int64_t
    string_int64_t(const std::string&);

    uint64_t
    string_uint64_t(const std::string&);

    f32_t
    string_f32_t(const std::string&);

    f64_t
    string_f64_t(const std::string&);

    /* Data */
    client_arg client_arguments_;
    std::string client_url_;

    /* Current jobs */
    std::vector<std::shared_ptr<JOB>> job_buffer_;   

    /* New jobs */
    std::vector<std::shared_ptr<JOB>> new_jobs_; 
    

};

/* Eof */