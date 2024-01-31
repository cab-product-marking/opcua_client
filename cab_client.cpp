#include "cab_client.h"

CAB_Client::CAB_Client() 
                : client_arguments_{DEFAULT_IP, 
                                    DEFAULT_PORT, 
                                    DEFAULT_USER, 
                                    DEFAULT_PASS, 
                                    false}
{
#ifdef FEATURE_CONSTRUCTOR_VISABLE
    std::cout   << FONT_GREEN << "CAB_Client::CAB_Client()" 
                << FONT_RESET << std::endl;
#endif // FEATURE_CONSTRUCTOR_VISABLE 
}

CAB_Client::~CAB_Client() 
{
#ifdef FEATURE_CONSTRUCTOR_VISABLE
    std::cout   << FONT_GREEN << "CAB_Client::~CAB_Client()" 
                << FONT_RESET << std::endl;   
#endif // FEATURE_CONSTRUCTOR_VISABLE     
}

int
CAB_Client::connect(const std::string &url)
{
    int state = {EXIT_FAILURE};
    return state = opcuac_connect(url, client_arguments_);
}

int 
CAB_Client::connect(int &argc, char *argv[])
{
    int state = {EXIT_FAILURE};
    std::string url = {"DEFAULT"};
    std::vector<std::string> job_list_buffer;
    /* At least one argument is required for the server uri */
    if(argc > 1)
    {   
        /* Parse the arguments */
        for(int argpos = 1; argpos < argc; argpos++) 
        {
            /* Parse the argument help */
            if(strcmp(argv[argpos], "--help") == 0 || strcmp(argv[argpos], "-h") == 0) 
            {
                CAB_Client::show_usage();
                return EXIT_FAILURE;
            }
            /* Parse the agrument url */
            if(strcmp(argv[argpos], "-url") == 0) 
            {
                argpos++;
                url = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--url=", 6) == 0)
            {
                url = std::string(argv[argpos] + 6);
                continue;
            }
            /* Parse the short arguments */
            if(strcmp(argv[argpos], "-u") == 0) 
            {
                argpos++;
                client_arguments_.username = std::string(argv[argpos]);
                continue;
            }
            /* Parse the long arguments */
            if(strncmp(argv[argpos], "--username=", 11) == 0)
            {
                client_arguments_.username = std::string(argv[argpos] + 11);
                continue;
            }
            if(strcmp(argv[argpos], "-p") == 0) 
            {
                argpos++;
                client_arguments_.password = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--password=", 11) == 0)
            {
                client_arguments_.username = std::string(argv[argpos] + 11);
                continue;
            }
            if(strcmp(argv[argpos], "-P") == 0) 
            {
                argpos++;
                client_arguments_.port = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--port=", 7) == 0)
            {
                client_arguments_.port = std::string(argv[argpos] + 7);
                continue;
            }
            if(strcmp(argv[argpos], "-I") == 0) 
            {
                argpos++;
                client_arguments_.server_ip = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--ip=", 5) == 0)
            {
                client_arguments_.server_ip = std::string(argv[argpos] + 5);
                continue;
            }
            if(strcmp(argv[argpos], "-m") == 0)
            {
                argpos++;
                client_arguments_.user_mode_enable = convert_string_to_bool(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--mode=", 7) == 0)
            {
                client_arguments_.user_mode_enable = convert_string_to_bool(argv[argpos] + 7);
                continue;
            }
            /* Parse for jobs */
            if (strcmp(argv[argpos], "-j") == 0)
            {
                argpos++;
                job_list_buffer.push_back(argv[argpos]);
                continue;
            }
            if (strncmp(argv[argpos], "--job=", 6) == 0)
            {
                job_list_buffer.push_back(argv[argpos] + 6);
                continue;
            }
        }
    }

    /* Build the current url */
    if(url == "DEFAULT")
    {
        /* Building the url with secure connection doesnt work in c-api */
        // if(client_arguments_.user_mode_enable == false)
        // {
        //     url = "opc.tcp://"  + client_arguments_.server_ip 
        //                         + ":" + client_arguments_.port;
        //     CAB_Client::set_url(url);
        // }
        // else
        // {
        //     url = "opc.tcp://"  + client_arguments_.username + ":" 
        //                         + client_arguments_.password + "@" 
        //                         + client_arguments_.server_ip + ":" 
        //                         + client_arguments_.port;
        //     CAB_Client::set_url(url);
        // }
        url = "opc.tcp://"  + client_arguments_.server_ip 
                            + ":" + client_arguments_.port;
        CAB_Client::set_url(url);
    }
    else
    {
        CAB_Client::set_url(url);
    }

    /* Only for memory leak testing with valgrind and file jobs.txt */
    // job_list_buffer.push_back("jobs.txt");

    /* Create all jobs from input */
    for(std::string &job_input : job_list_buffer)
    {
        if(find_file_in_directory(RES_FILEPATH, job_input))
        {
            std::string file_path = RES_FILEPATH + std::string(job_input);

            print_info("Use jobs from file: " << file_path);
            std::ifstream file(file_path);
            std::string line;
            if(file.is_open())
            {
                while(std::getline(file, line))
                {
                    size_t pos = line.find('#');
                    if(pos != std::string::npos)
                    {
                        line = line.substr(0, pos);
                    }
                    if(!line.empty())
                    {
                        job_create(line.c_str());
                    }
                }
                file.close();
            }
            else
            {
                print_error("Can not open file: " << file_path);
            }
        }
        else
        {
            job_create(job_input);
        }
    }
    
    /* Set default time job for test */
    if(job_buffer_.empty() == true)
    {
        /* Insert default jobs for testing */
        job_create("node_read:ID_HEAT_LEVEL:4");
    }

    print_job_list();
    opcuac_init();
    return state = opcuac_connect(url, client_arguments_);
}

void
CAB_Client::disconnect()
{
    opcuac_disconnect();
#ifdef __linux__
    CAB_Client::_cab(RESPONSE_COLUMN);
#endif // __linux__
    return;
}

void
CAB_Client::run_iterate()
{
    if(opcuac_run_iterate() == EXIT_SUCCESS)
    {
        if(job_buffer_.empty() && new_jobs_.empty())
        {
            // struct timeval tv;
            // gettimeofday(&tv, nullptr);
            // if(tv.tv_usec % 2 == 0)
            // {
            //     std::cout.flush()   << CURSOR_COL(0) 
            //                         << "Iterate - job list empty."
            //                         << " \\";
            // }
            // else
            // {
            //     std::cout.flush()   << CURSOR_COL(0)
            //                         << "Iterate - job list empty."
            //                         << " /";
            // }
            print_info("Iterate - job list empty.");
        }
        else
        {
            if(!new_jobs_.empty())
            {
                print_info("received new jobs.");
                /* Iterate and insert new jobs */
                for(auto it = new_jobs_.begin(); it != new_jobs_.end(); ++it)
                {
                    job_buffer_.insert(job_buffer_.begin(), *it);
                }
                new_jobs_.clear();
            }
            /* Main iterate loop */
            print_info("Iterate - working ... ");
            for(auto actual_job = job_buffer_.begin(); 
                actual_job < job_buffer_.end(); /* ++actual_job */)
            {
                switch ((*actual_job)->type)
                {
                case job_type::mitem_add:
                    add_monitored_item(*actual_job);
                    break;
                case job_type::mitem_delete:
                    delete_monitored_item(*actual_job);
                    break;
                case job_type::node_read:
                    read_node(*actual_job);
                    break;
                case job_type::node_write:
                    write_node(*actual_job);
                    break;
                case job_type::browse:
                    browse_nodes(*actual_job);
                    break;
                case job_type::print:
                    print_label(*actual_job);
                    break;
                case job_type::replace:
                    replace_label(*actual_job);
                    break;
                case job_type::DEFAULT:
                    break;
                default:
                    break;
                }
                
                /* Delete actual job and iterate when delete */
                if((*actual_job)->erase == true)
                {
                    print_info("delete job.");
                    actual_job = job_buffer_.erase(actual_job);
                }
                else
                {
                    /* Increment manualy when job persists */
                    ++actual_job;
                }
            }
            print_info("end iterate loop.");
        }
    }
    return;
}

void
CAB_Client::job_create(const std::string input,
                        std::shared_ptr<DATA> shared_data)
{
    std::string input_string = input;
    if(input_string.empty())
    {
        print_info("input_string empty.");
        return;
    }

    /* Find elements */
    std::map<int, std::string> input_map = parse_arguments(input_string);
    if(input_map.empty())
    {
        print_info("input_map empty.");
        return;
    }

    std::shared_ptr<JOB> job = std::make_shared<JOB>(init_job());
    job->init_string = input_string;
    
    auto element0 = *input_map.begin();
    job->type = get_job_type(element0.second);

    /* Node id, type_id and namespace settings */
    if(!(job->type == job_type::print || 
         job->type == job_type::replace))
    {
        if(input_map.size() > 1)
        {
            /* Id and type_id*/
            auto element1 = *input_map.find(1);
            if(CAB_Client::request_all_digits(element1.second))
            {
                job->id_numeric = std::stoi(element1.second);
                job->type_id = id_type::numeric;
            }
            else
            {
                job->id_string = element1.second;
                job->type_id = id_type::string;
            }

            /* Namespace */
            auto element2 = *input_map.find(2);
            job->namespace_index = std::stoi(element2.second);
        }
    }
    
    /* Workflow depends on job type */
    if(job->type == job_type::node_write)
    {
        /* Data type */
        auto element3 = *input_map.find(3);
        job->intern_data->type_data = get_data_type(element3.second);
        if(job->intern_data->type_data == data_type::DEFAULT)
        {
            print_error("Invalid write input format found.");
            return;
        }
        
        /* Data */
        std::string data;
        for(auto it = input_map.find(4); it != input_map.end(); ++it)
        {
            if(it != input_map.find(4))
            {
                data += ":";
            }
            data += it->second;
        }
        set_data_value(data, job);
    }
    else if(job->type == job_type::print)
    {
        /* Find language type */
        job->intern_data->type_data = data_type::c_boolean;
        auto element1 = *input_map.find(1);
        if(element1.second == "zpl")
        {
            element1.second = "true";
        }
        else
        {
            element1.second = "false";
        }
        set_data_value(element1.second, job);
    
        /* Find label content without validation */
        job->intern_data->type_data = data_type::c_string;
        auto element2 = *input_map.find(2);
        if(find_file_in_directory(RES_FILEPATH, element2.second))
        {
            std::string temp_path = RES_FILEPATH + element2.second;

            print_info("Use label content from file; " << temp_path);
            std::ifstream file(temp_path);
            if(file.is_open())
            {
                std::stringstream string_stream;
                string_stream << file.rdbuf();
                job->intern_data->string_value = string_stream.str() + "\n";
                file.close();
            }
            else
            {
                print_error("Can not open file; " << temp_path);
            }
        }
        else if(element2.second == "PrintNow")
        {
            job->intern_data->u16_value = 1;
        }
        else
        {
            job->intern_data->string_value = element2.second + "\n";
        }
        /* Find bitmap or more JScript */
        if(input_map.size() > 3)
        {
            auto element3 = *input_map.find(3);
            if(find_file_in_directory(RES_FILEPATH, element3.second) == true)
            {
                job->intern_data->file_name = RES_FILEPATH + element3.second;
            }
            else
            {
                std::string str;
                for(auto it = input_map.find(2); it != input_map.end(); ++it)
                {
                    if(!str.empty())
                    {
                        str += ":";
                    }
                    str += it->second;
                }
                job->intern_data->string_value = str + "\n";
            }
        }
    }
    else if(job->type == job_type::replace)
    {
        /* Get data for replacement */
        if(input_map.size() < 3 || !(input_map.size() % 2))
        {
            print_error("you entered replace with invalid arguments.");
            return;
        }
        /* Parse replacement data */
        std::map<std::string, std::string> replacements;
        for(auto it = input_map.find(1); it != input_map.end(); ++it)
        {
            std::string dummy = it->second;
            if (++it == input_map.end()) 
            {
                break;
            }
            replacements.emplace(dummy, it->second);
        }
        job->intern_data->replace = replacements;
    }
    else if(job->type == job_type::DEFAULT)
    {
        print_info("job_type is invalid. No job was create.");
        return;
    }

    /* Use extern DATA space? */
    if(shared_data != nullptr)
    {
        job->intern_data = shared_data;
        new_jobs_.push_back(job);
        return;
    }
    
    /* Push back actual job configuration */
    job_buffer_.push_back(job);
    return;
}

void
CAB_Client::add_monitored_item(std::shared_ptr<JOB> job)
{
    print_info("add_monitored_item()" << job->init_string);
    print_console_message(job, "add monitored item");
    opcuac_add_monitored_item(job);
    /* Erase from job list */
    job->erase = true;
    return;
}

void
CAB_Client::delete_monitored_item(std::shared_ptr<JOB> job)
{
    print_info("delete_monitored_item()" << job->init_string);
    print_console_message(job, "delete monitored item");
    opcuac_delete_monitored_item(job);
    /* Erase from job list */
    job->erase = true;
    return;
}

void 
CAB_Client::read_node(std::shared_ptr<JOB> job)
{
    print_info("read_node()" << job->init_string);
    opcuac_read_node(job);
    print_console_message_data(job, "read response");
    /* Erase from job list */
    job->erase = true;
    return;
}

void
CAB_Client::write_node(std::shared_ptr<JOB> job)
{
    print_info("write_node()" << job->init_string);
    std::shared_ptr<DATA> store_job_data = job->intern_data;
    opcuac_read_node(job);
    if(job->intern_data->type_data == store_job_data->type_data)
    {
        job->intern_data = store_job_data;
        opcuac_write_node(job);
        print_console_message_data(job, "write request");
    }
    else
    {
        print_error("entered wrong data type.");
    }
    /* Erase from job list */
    job->erase = true;
    return;
}

void
CAB_Client::browse_nodes(std::shared_ptr<JOB> job)
{
    print_info("browse_nodes()" << job->init_string);
    print_console_message(job, "browse nodes with Tree_Node objects");
    opcuac_browse_nodes(job, client_url_);
    /* Erase from job list */
    job->erase = true;
    return;
}

void 
CAB_Client::print_label(std::shared_ptr<JOB> job)
{
    print_info("print_label()" << job->init_string);
    print_console_message(job, "print label with JScript/zpl");
    if(job->intern_data->u16_value == 1)
    {
        /* Print */
        /* Enter amount of labels */
        job->intern_data->u16_value = 1;
        opcuac_call_method_PrintCurrentLabel(job);
    }
    else if(job->intern_data->file_name.empty())
    {
        CAB_Client::upload_stream(job);
    }
    else
    {
        opcuac_call_method_FileUpload(job);
        CAB_Client::upload_stream(job);
    }
    /* Erase from job list */
    job->erase = true;
    return;
}

void 
CAB_Client::replace_label(std::shared_ptr<JOB> job)
{
    /* Label with replace contents needs to load or print before */
    if(job->intern_data->u32_value == 0)
    {
        print_info("replace_label()" << job->init_string);
        print_console_message(job, "start replace items in label procedure");
        /* Need to know then contents item you want to replace */
        std::shared_ptr<JOB> node = std::make_shared<JOB>(init_job());
        for(auto it = job->intern_data->replace.begin(); 
            it != job->intern_data->replace.end(); ++it)
        {
            /* Node settings */
            node->type_id = id_type::string;
            node->id_string = it->first;
            node->namespace_index = 5;
            /* Data settings */
            node->intern_data->type_data = data_type::c_string;
            node->intern_data->string_value = it->second;
            
            if(opcuac_write_node(node) != EXIT_SUCCESS)
            {
                print_info("write replace contents failed.");
                job->erase = true;
                return;
            }
        }
        job->intern_data->u32_value++;
        return;
    }
    if(job->intern_data->u32_value == 1)
    {
        /* Also possible with print:JScript:PrintNow */
        /* Amount of labels */
        job->intern_data->u16_value = 1;
        opcuac_call_method_PrintCurrentLabel(job);

        /* Erase from job list */
        job->erase = true;
        return;
    }
}

JOB
CAB_Client::init_job(void)
{
    JOB j;
    /* JOB init */
    j.type = job_type::DEFAULT;        
    j.type_id = id_type::DEFAULT;      
    j.id_string = "DEFAULT";                  
    j.id_numeric = - 1;                   
    j.namespace_index = - 1;
    j.init_string = "DEFAULT";
    j.erase = false;
    /* Data */
    j.intern_data = init_data();
    /* Extern data */
    j.extern_data = init_data();
    return j;
}   

std::shared_ptr<DATA>
CAB_Client::init_data(void)
{
    std::shared_ptr<DATA> j = std::make_shared<DATA>();
    /* DATA init */
    j->type_data = data_type::DEFAULT;
    j->time = {0, 0, 0, 0, 0, 0, 70, 0, 0, 0, 0, nullptr};  /* Unix as default */
    j->b_value = false;
    j->s16_value = 0;
    j->u16_value = 0;
    j->s32_value = 0;
    j->u32_value = 0;
    j->s64_value = 0;
    j->u64_value = 0;
    j->f32_value = 0.0f;
    j->f64_value = 0.0;
    j->string_value.clear();
    j->file_name.clear();
    j->replace.clear();
    return j;
}

void
CAB_Client::show_usage(void)
{
    /* Usage */
    std::string file_path = RES_FILEPATH + std::string("usage.txt");
    std::ifstream file(file_path);
    if(!file.is_open())
    {
        print_error("Can not open file; " << file_path);
        return;
    }
    else
    {
        std::string line;
        while(getline(file, line))
        {
            std::cout << line << std::endl;
        }
    }
    file.close();
    return;
}

void
CAB_Client::upload_stream(std::shared_ptr<JOB> job)
{
    /* Guarantees that labels or files has been uploaded completely */
    /* Check upload status - Current Label Definition Completion Sequence */
    // print_info(job->intern_data->string_value);
    /* Depends on job syntax, no handling for zpl implemented */
    /* feed? */
    if(job->intern_data->string_value == "f\n")
    {
        /* Method call without monitoring */
        opcuac_call_method_PrintData(job);
        return;
    }
    else
    {
        std::shared_ptr<JOB> node = std::make_shared<JOB>(init_job());
        node->type_id = id_type::numeric;
        node->id_numeric = 10086;
        node->namespace_index = 3;

        opcuac_read_node(node);
        int start = node->intern_data->u32_value;
        int end = start;
        print_info("start counter: " << start);

        /* Monitored method call */
        opcuac_call_method_PrintData(job);
        int breaker = 0;
        do
        {
            breaker++;
            opcuac_read_node(node);
            end = node->intern_data->u32_value;
            print_info("end counter: " << end << " - breaker = " << breaker << "/100");
        } while (end == start && breaker < 100);
        if(breaker == 100)
        {
            print_error("breaker stopped loop");
        }
        return;
    }
}

bool
CAB_Client::find_file_in_directory(const std::string &directory, 
                        const std::string &file_name)
{
    std::string str;
    try 
    {
        if(directory == "current")
        {
            str = std::filesystem::current_path().string();
        }
        else
        {
            str = directory;
        }

        /* Check the directory */
        if (!std::filesystem::exists(str) || !std::filesystem::is_directory(str)) 
        {
            print_error("directory invalid: " << str);
            return false;
        }
        /* Iterate inside directory */
        for (const auto& entry : std::filesystem::directory_iterator(str)) 
        {
            if (entry.is_regular_file() && entry.path().filename() == file_name) 
            {
                return true;
            }
        }
    } 
    catch(const std::filesystem::filesystem_error& e) 
    {
        print_error("system directory error");
    }
    return false;
}

std::map<int, std::string>
CAB_Client::parse_arguments(const std::string &input)
{
    std::string str = input;
    std::map<int, std::string> result;
    result.clear();

    /* Erase empty elements in string */
    size_t endpos = str.find_last_not_of(" \t");
    str = str.substr(0, endpos + 1);

    /* Last char is ':' */
    if(str.back() == ':')
    {
        print_error("syntax error.")
        return result;
    }

    std::stringstream ss(input);
    std::string item;
    int index = 0;
    while(std::getline(ss, item, ':'))
    {
        if(item.empty())
        {
            print_error("single argument is empty.");
            std::map<int, std::string> end;
            return end;
        }
        // result[index++] = item;
        result.emplace(index++, item);
    }    
    return result;
}

bool
CAB_Client::convert_string_to_bool(const std::string &string)
{
    bool state = {false};
    if(strcmp(string.c_str(), "user") == 0)
    {
        state = true;
    }
    return state;
}

bool 
CAB_Client::request_all_digits(const std::string &str) 
{
    for(char c : str) 
    {
        if(!std::isdigit(c)) 
        {
            return false;
        }
    }
    return true;
}

job_type
CAB_Client::get_job_type(std::string &string)
{
    job_type job = job_type::DEFAULT;
    if      (string == "mitem_add") return job = job_type::mitem_add;
    else if (string == "mitem_delete") return job = job_type::mitem_delete;
    else if (string == "node_read") return job = job_type::node_read;
    else if (string == "node_write") return job = job_type::node_write;
    else if (string == "browse") return job = job_type::browse;
    else if (string == "print") return job = job_type::print;
    else if (string == "replace") return job = job_type::replace;
    else return job = job_type::DEFAULT;
}

job_type
CAB_Client::get_job_type(std::shared_ptr<JOB> job)
{
    return job->type;
}

std::string 
CAB_Client::get_job_type_string(const job_type &type)
{
    if      (type == job_type::mitem_add) return "mitem_add";
    else if (type == job_type::mitem_delete) return "mitem_delete";
    else if (type == job_type::node_read) return "node_read";
    else if (type == job_type::node_write) return "node_write";
    else if (type == job_type::browse) return "browse";
    else if (type == job_type::print) return "print";
    else if (type == job_type::replace) return "replace";
    else return "DEFAULT";
}

std::string
CAB_Client::get_id_type_string(const id_type &type)
{
    if      (type == id_type::string) return "string";
    else if (type == id_type::numeric) return "numeric";
    else return "DEFAULT";
}

std::string
CAB_Client::get_data_type_string(const data_type &type)
{
    if      (type == data_type::c_datetime) return "datetime";
    else if (type == data_type::c_boolean) return "bool";
    else if (type == data_type::c_int16) return "int16_t";
    else if (type == data_type::c_uint16) return "uint16_t";
    else if (type == data_type::c_int32) return "int32_t";
    else if (type == data_type::c_uint32) return "uint32_t";
    else if (type == data_type::c_int64) return "int64_t";
    else if (type == data_type::c_uint64) return "uint64_t";
    else if (type == data_type::c_f32) return "f32_t";
    else if (type == data_type::c_f64) return "f64_t";
    else if (type == data_type::c_string) return "string";
    else if (type == data_type::DEFAULT) return "DEFAULT";
    else return "DEFAULT";
}

data_type
CAB_Client::get_data_type(const std::string &data_type)
{
    if      (data_type == "datetime") return data_type::c_datetime;
    else if (data_type == "bool") return data_type::c_boolean;
    else if (data_type == "int16_t") return data_type::c_int16;
    else if (data_type == "uint16_t") return data_type::c_uint16;
    else if (data_type == "int32_t") return data_type::c_int32;
    else if (data_type == "uint32_t") return data_type::c_uint32;
    else if (data_type == "int64_t") return data_type::c_int64;
    else if (data_type == "uint64_t") return data_type::c_uint64;
    else if (data_type == "f32_t") return data_type::c_f32;
    else if (data_type == "f64_t") return data_type::c_f64;
    else if (data_type == "string") return data_type::c_string;
    else if (data_type == "DEFAULT") return data_type::DEFAULT;
    else return data_type::DEFAULT;
}

int
CAB_Client::int_from_string(std::string &string)
{
    // std::cout << string << " | " << string.size() << std::endl;
    if(request_all_digits(string) == true)
    {
        return std::stoi(string);
    }
    return - 1;
}

std::string
CAB_Client::get_url(void)
{
    return CAB_Client::client_url_;
}

void
CAB_Client::set_url(std::string &url)
{
    CAB_Client::client_url_ = url;
    return;
}

void
CAB_Client::set_data_value(const std::string &data_value,
                           std::shared_ptr<JOB> &job)
{
    switch (job->intern_data->type_data)
    {
    case data_type::c_datetime:
        job->intern_data->time = string_data_time(data_value);
        break;
    case data_type::c_boolean:
        job->intern_data->b_value = string_bool(data_value);
        break;
    case data_type::c_int16:
        job->intern_data->s16_value = string_int16_t(data_value);
        break;
    case data_type::c_uint16:
        job->intern_data->u16_value = string_uint16_t(data_value);
        break;
    case data_type::c_int32:
        job->intern_data->s32_value = string_int32_t(data_value);
        break;
    case data_type::c_uint32:
        job->intern_data->u32_value = string_uint32_t(data_value);
        break;
    case data_type::c_int64:
        job->intern_data->s64_value = string_int64_t(data_value);
        break;
    case data_type::c_uint64:
        job->intern_data->u64_value = string_uint64_t(data_value);
        break;
    case data_type::c_f32:
        job->intern_data->f32_value = string_f32_t(data_value);
        break;
    case data_type::c_f64:
        job->intern_data->f64_value = string_f64_t(data_value);
        break;
    case data_type::c_string:
        job->intern_data->string_value = data_value;
        break;
    case data_type::DEFAULT:
        break;
    default:
        break;
    }
    return;
}

std::string
CAB_Client::get_data_value_string(std::shared_ptr<JOB> &job)
{
    std::string time_as_string;
    std::ostringstream oss;
    switch (job->intern_data->type_data)
    {
    case data_type::c_datetime:
        oss << std::setfill('0')
            << std::setw(2) << job->intern_data->time.tm_mday << '.'
            << std::setw(2) << job->intern_data->time.tm_mon + 1 << '.'
            << std::setw(4) << job->intern_data->time.tm_year + 1900 << " | "
            << std::setw(2) << job->intern_data->time.tm_hour << ':'
            << std::setw(2) << job->intern_data->time.tm_min << ':'
            << std::setw(2) << job->intern_data->time.tm_sec << ':'
            << std::setw(3) << job->intern_data->time.tm_msec;
        time_as_string = oss.str();
        return time_as_string;      
        break;
    case data_type::c_boolean:
        return std::to_string(job->intern_data->b_value);
        break;
    case data_type::c_int16:
        return std::to_string(job->intern_data->s16_value);
        break;
    case data_type::c_uint16:
        return std::to_string(job->intern_data->u16_value);
        break;
    case data_type::c_int32:
        return std::to_string(job->intern_data->s32_value);
        break;
    case data_type::c_uint32:
        return std::to_string(job->intern_data->u32_value);
        break;
    case data_type::c_int64:
        return std::to_string(job->intern_data->s64_value);
        break;
    case data_type::c_uint64:
        return std::to_string(job->intern_data->u64_value);
        break;
    case data_type::c_f32:
        return std::to_string(job->intern_data->f32_value);
        break;
    case data_type::c_f64:
        return std::to_string(job->intern_data->f64_value);
        break;
    case data_type::c_string:
        return job->intern_data->string_value;
        break;
    case data_type::DEFAULT:
        return "DEFAULT";
        break;
    default:
        break;
    }
    return "DEFAULT";
}

void
CAB_Client::print_job_list(void)
{
#ifdef FEATURE_DEBUG_INFO  
    print_info("print job list." << std::endl << CONSOLE_STARS_50);
    if(job_buffer_.empty() == true)
    {
        print_info("job list empty.");
    }
    else
    {
        for(std::shared_ptr<JOB> &dummy : job_buffer_)
        {
            print_job(dummy);
        }
    }
#endif // FEATURE_DEBUG_INFO
    return;
}

void
CAB_Client::print_job(std::shared_ptr<JOB> &job)
{
    if(get_job_type(job) == job_type::node_write)
    {
        std::cout   << "type: " << get_job_type_string(job->type) << std::endl
                    << "type_id: " << get_id_type_string(job->type_id) << std::endl
                    << "id_string: " << job->id_string << std::endl
                    << "id_numeric: " << job->id_numeric << std::endl
                    << "ns: " << job->namespace_index << std::endl
                    << "init_string: " << job->init_string << std::endl
                    << "data_type: " << get_data_type_string(job->intern_data->type_data) << std::endl
                    << "data_value: " << get_data_value_string(job) << std::endl
                    << CONSOLE_STARS_50 
                    << std::endl;
    }
    else if(get_job_type(job) == job_type::print)
    {
        std::string sub_string = {""};
        if(get_data_value_string(job).length() > 50)
        {
            sub_string = get_data_value_string(job).substr(0, 50) + "\n ...";
        }
        else
        {
            sub_string = get_data_value_string(job) + " ...";
        }
        std::cout   << "type: " << get_job_type_string(job->type) << std::endl
                    << "type_id: " << get_id_type_string(job->type_id) << std::endl
                    << "id_string: " << job->id_string << std::endl
                    << "id_numeric: " << job->id_numeric << std::endl
                    << "ns: " << job->namespace_index << std::endl
                    << "init_string: " << job->init_string << std::endl
                    << "data_type: " << get_data_type_string(job->intern_data->type_data) << std::endl
                    << "data_value: \n" << sub_string << std::endl
                    << "zpl: " << job->intern_data->b_value << std::endl
                    << "bitmap: " << job->intern_data->file_name << std::endl
                    << CONSOLE_STARS_50 
                    << std::endl;
    }
    else if(get_job_type(job) == job_type::replace)
    {
        std::cout   << "type: " << get_job_type_string(job->type) << std::endl
                    << "type_id: " << get_id_type_string(job->type_id) << std::endl
                    << "id_string: " << job->id_string << std::endl
                    << "id_numeric: " << job->id_numeric << std::endl
                    << "ns: " << job->namespace_index << std::endl
                    << "init_string: " << job->init_string << std::endl
                    << "data_value: " << std::endl;
        for(auto it = job->intern_data->replace.begin(); it != job->intern_data->replace.end(); ++it)
        {
            std::cout   << "Replace " 
                        << it->first 
                        << " contents with " 
                        << FONT_LIGHT_PURPLE
                        << it->second 
                        << FONT_RESET
                        << std::endl;
        }
        std::cout   << CONSOLE_STARS_50 
                    << std::endl;            

                    
    }
    else
    {
        std::cout   << "type: " << get_job_type_string(job->type) << std::endl
                    << "type_id: " << get_id_type_string(job->type_id) << std::endl
                    << "id_string: " << job->id_string << std::endl
                    << "id_numeric: " << job->id_numeric << std::endl
                    << "ns: " << job->namespace_index << std::endl
                    << "init_string: " << job->init_string << std::endl
                    << CONSOLE_STARS_50 
                    << std::endl;
    }
    return;
}

void
CAB_Client::print_console_message(std::shared_ptr<JOB> &job,
                        std::string type)
{
    std::cout   << CAB_USER
                << " - "
                << job->init_string 
                << " - "
                << type 
                << std::endl;
    return;
}

void
CAB_Client::print_console_message_data(std::shared_ptr<JOB> &job, 
                        std::string type)
{
    std::cout << FONT_LIGHT_BLUE;
    switch (job->intern_data->type_data)
    {
    case data_type::c_datetime:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    /* Date */
                    << std::setfill('0') 
                    << std::setw(2) << job->intern_data->time.tm_mday << "."
                    << std::setw(2) << job->intern_data->time.tm_mon + 1 << "."
                    << std::setw(4) << job->intern_data->time.tm_year + 1900 << " | "
                    /* Time */
                    << std::setw(2) << job->intern_data->time.tm_hour << ":"
                    << std::setw(2) << job->intern_data->time.tm_min << ":"
                    << std::setw(2) << job->intern_data->time.tm_sec << ":"
                    << std::setw(3) << job->intern_data->time.tm_msec
                    << std::endl;
        break;
    case data_type::c_boolean:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->b_value
                    << std::endl;
        break;
    case data_type::c_int16:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->s16_value
                    << std::endl;
        break;
    case data_type::c_uint16:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->u16_value
                    << std::endl;
        break;
    case data_type::c_int32:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->s32_value
                    << std::endl;
        break;
    case data_type::c_uint32:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - " 
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->u32_value
                    << std::endl;
        break;
    case data_type::c_int64:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->s64_value
                    << std::endl;
        break;
    case data_type::c_uint64:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->u64_value
                    << std::endl;
        break;
    case data_type::c_f32:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->f32_value
                    << std::endl;
        break;
    case data_type::c_f64:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->f64_value
                    << std::endl;
        break;
    case data_type::c_string:
        std::cout << CAB_USER
                    << " - " 
                    << job->init_string << " - "
                    << type << ": "
                    << "(" << get_data_type_string(job->intern_data->type_data) << ") "
                    /* Data */
                    << job->intern_data->string_value
                    << std::endl;
        break;
    case data_type::DEFAULT:
        break;
    default:
        break;
    }
    std::cout << FONT_RESET;
    return;
}

data_time
CAB_Client::string_data_time(const std::string& string)
{
    data_time time = get_system_time();
    
    if(string == "actual")
    {
        return time; 
    }
    else
    {
        std::map<int, std::string> input = parse_arguments(string);
        if(input.size() != 7)
        {
            print_info("argument syntax invalid. return actual time.")
            return time;
        }

        /* Input Syntax day:month:year:hour:min:sec:msec */
        auto e1 = *input.find(0);
        auto e2 = *input.find(1);
        auto e3 = *input.find(2);
        auto e4 = *input.find(3);
        auto e5 = *input.find(4);
        auto e6 = *input.find(5);
        auto e7 = *input.find(6);

        /* No data validation */
        time.tm_mday = std::stoi(e1.second);
        time.tm_mon = std::stoi(e2.second) - 1;
        time.tm_year = std::stoi(e3.second) - 1900;
        time.tm_hour = std::stoi(e4.second);
        time.tm_min = std::stoi(e5.second);
        time.tm_sec = std::stoi(e6.second);
        time.tm_msec = std::stoi(e7.second);

        print_info("Time input: "
                << time.tm_mday << '.'
                << time.tm_mon << '.'
                << time.tm_year << " | "
                << time.tm_hour << ':'
                << time.tm_min << ':'
                << time.tm_sec << ':'
                << time.tm_msec);

        return time;
    }
}

data_time CAB_Client::get_system_time(void) 
{
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm now_tm;

    localtime_s(&now_tm, &t); 

    data_time dt;
    dt.tm_msec = static_cast<int>(milliseconds.count() % 1000);
    dt.tm_sec = now_tm.tm_sec;
    dt.tm_min = now_tm.tm_min;
    dt.tm_hour = now_tm.tm_hour;
    dt.tm_mday = now_tm.tm_mday;
    dt.tm_mon = now_tm.tm_mon + 1;
    dt.tm_year = now_tm.tm_year + 1900;

    print_info("Time: "
            << dt.tm_mday << '.'
            << dt.tm_mon << '.'
            << dt.tm_year << " | "
            << dt.tm_hour << ':'
            << dt.tm_min << ':'
            << dt.tm_sec << ':'
            << dt.tm_msec);
    return dt;
}

//data_time 
//CAB_Client::get_system_time(void) 
//{
//    auto now = std::chrono::system_clock::now();
//    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
//
//    time_t t = std::chrono::system_clock::to_time_t(now);
//    struct tm* now_tm = localtime_s(&t);
//
//    data_time dt;
//    dt.tm_msec = milliseconds.count() % 1000;
//    dt.tm_sec = now_tm->tm_sec;
//    dt.tm_min = now_tm->tm_min;
//    dt.tm_hour = now_tm->tm_hour;
//    dt.tm_mday = now_tm->tm_mday;
//    dt.tm_mon = now_tm->tm_mon + 1;  
//    dt.tm_year = now_tm->tm_year + 1900;  
//
//    print_info("Time: " << dt.tm_mday << '.' << dt.tm_mon << '.' << dt.tm_year << " | "
//        << dt.tm_hour << ':' << dt.tm_min << ':' << dt.tm_sec << '.' << dt.tm_msec);
//
//    return dt;
//}

    
//data_time   
//CAB_Client::get_system_time(void) 
//{
//    struct timeval tv;
//    gettimeofday(&tv, NULL);
//
//    struct tm *now = localtime(&tv.tv_sec);
//
//    data_time dt;
//    dt.tm_msec = tv.tv_usec / 1000; 
//    dt.tm_sec = now->tm_sec;
//    dt.tm_min = now->tm_min;
//    dt.tm_hour = now->tm_hour;
//    dt.tm_mday = now->tm_mday;
//    dt.tm_mon = now->tm_mon;
//    dt.tm_year = now->tm_year;
//
//    print_info("Time linux: "
//                << dt.tm_mday << '.'
//                << dt.tm_mon << '.'
//                << dt.tm_year << " | "
//                << dt.tm_hour << ':'
//                << dt.tm_min << ':'
//                << dt.tm_sec << ':'
//                << dt.tm_msec);
//    return dt;
//}

bool 
CAB_Client::string_bool(const std::string& string)
{
    if(string == "true" || string == "1")
    {
        return true;
    }
    else if(string == "false" || string == "0")
    {
        return false;
    }
    else
    {
        return false;
    }
}

int16_t
CAB_Client::string_int16_t(const std::string& string)
{
    return static_cast<int16_t>(std::stoi(string));
}

uint16_t
CAB_Client::string_uint16_t(const std::string& string)
{
    return static_cast<uint16_t>(std::stoul(string));
}

int32_t 
CAB_Client::string_int32_t(const std::string& string)
{
    return static_cast<int32_t>(std::stoi(string));
}

uint32_t 
CAB_Client::string_uint32_t(const std::string& string)
{
    return static_cast<uint32_t>(std::stoul(string));
}

int64_t
CAB_Client::string_int64_t(const std::string& string)
{
    return static_cast<int64_t>(std::stoi(string));
}

uint64_t
CAB_Client::string_uint64_t(const std::string& string)
{
    return static_cast<uint64_t>(std::stoul(string));
}

f32_t
CAB_Client::string_f32_t(const std::string& string)
{
    return std::stof(string);
}

f64_t
CAB_Client::string_f64_t(const std::string& string)
{
    return std::stod(string);
}

/* Eof */