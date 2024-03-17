#include "client.h"

using namespace cab;

Client::Client()
{
#ifndef FEATURE_CLIENT_OFF
    client_ = new open62541::Client();
#endif // !FEATURE_CLIENT_OFF
}

Client::~Client()
{
#ifndef FEATURE_CLIENT_OFF
    delete client_;
#endif // !FEATURE_CLIENT_OFF
}

int 
Client::connect(int &argc, char *argv[])
{
    int state = EXIT_FAILURE;

    std::vector<std::string> job_buffer;

    open62541::server_arguments arg;
    
    /* Parse the input arguments */
    if(argc > 1)
    {   
        for(int argpos = 1; argpos < argc; argpos++) 
        {
            if(strcmp(argv[argpos], "--help") == 0 || strcmp(argv[argpos], "-h") == 0) 
            {
                Client::show_usage();
                return EXIT_FAILURE;
            }
            if(strcmp(argv[argpos], "-url") == 0) 
            {
                argpos++;
                arg.server_url = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--url=", 6) == 0)
            {
                arg.server_url = std::string(argv[argpos] + 6);
                continue;
            }
            /* Parse the short arguments */
            if(strcmp(argv[argpos], "-u") == 0) 
            {
                argpos++;
                arg.server_username = std::string(argv[argpos]);
                continue;
            }
            /* Parse the long arguments */
            if(strncmp(argv[argpos], "--username=", 11) == 0)
            {
                arg.server_username = std::string(argv[argpos] + 11);
                continue;
            }
            if(strcmp(argv[argpos], "-p") == 0) 
            {
                argpos++;
                arg.server_password = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--password=", 11) == 0)
            {
                arg.server_password = std::string(argv[argpos] + 11);
                continue;
            }
            if(strcmp(argv[argpos], "-P") == 0) 
            {
                argpos++;
                arg.server_port = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--port=", 7) == 0)
            {
                arg.server_port = std::string(argv[argpos] + 7);
                continue;
            }
            if(strcmp(argv[argpos], "-I") == 0) 
            {
                argpos++;
                arg.server_ip = std::string(argv[argpos]);
                continue;
            }
            if(strncmp(argv[argpos], "--ip=", 5) == 0)
            {
                arg.server_ip = std::string(argv[argpos] + 5);
                continue;
            }
            if(strcmp(argv[argpos], "-m") == 0)
            {
                argpos++;
                std::string str = argv[argpos];
                if(strcmp(str.c_str(), "secure") == 0)
                {
                    arg.server_secure_mode = true;
                }
                continue;
            }
            if(strncmp(argv[argpos], "--mode=", 7) == 0)
            {
                std::string str = argv[argpos] + 7;
                if(strcmp(str.c_str(), "secure") == 0)
                {
                    arg.server_secure_mode = true;
                }
                continue;
            }
            /* Parse for jobs */
            if (strcmp(argv[argpos], "-j") == 0)
            {
                argpos++;
                job_buffer.push_back(argv[argpos]);
                continue;
            }
            if (strncmp(argv[argpos], "--job=", 6) == 0)
            {
                job_buffer.push_back(argv[argpos] + 6);
                continue;
            }
        }
    }
#ifndef FEATURE_TESTING
    else
    {
        cLOG(Level::INFO, "You have not entered any arguments. Default? <Y/N>");
        std::string str;
        str.clear();
        std::cin >> str;
        if(str == "Y" || str == "y")
        {
            job_buffer.push_back("node_read:ID_HEAT_LEVEL:4");
        }
        else
        {
            return state;
        }
    }
#else
    else
    {
        cLOG(Level::INFO, "Insert jobs.txt as default.");
        job_buffer.push_back("jobs.txt");
    }
#endif // FEATURE_TESTING 
   
    /* Build current url */
    if(arg.server_url == "DEFAULT")
    {
        arg.server_url = "opc.tcp://"  + arg.server_ip + ":" + arg.server_port;
    }
    /* Secure connection only possible with secure function call, not with secure url */

    /* If file, check directory for more jobs */
    for(auto &str : job_buffer)
    {
        if(file_finder(DIR_RES, str))
        {
            std::string file_path = DIR_RES + std::string(str);

            cLOG(Level::INFO, "Use jobs from file: " + file_path);
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
                        create_job(line.c_str());
                    }
                }
                file.close();
            }
            else
            {
                cLOG(Level::ERROR, "Can not open file: " + file_path);
            }
        }
        else
        {
            create_job(str);
        }
    }

    /* Connect client */
#ifdef FEATURE_CLIENT_OFF
    state = EXIT_SUCCESS;
#else
#ifdef FEATURE_TESTING
    print_jobs_();
#endif // FEATURE_TESTING
    client_->init();
    state = client_->connect(arg);
#endif // FEATURE_CLIENT_OFF

return state;
}

void
Client::disconnect()
{
#ifndef FEATURE_CLIENT_OFF
    client_->disconnect();
#endif // !FEATURE_CLIENT_OFF
    return;
}

void
Client::run_iterate()
{
#ifndef FEATURE_CLIENT_OFF
    if(client_->run_iterate() == EXIT_SUCCESS)
    {
        if(this->jobs_.empty())
        {
            cLOG(Level::INFO, "Iterate - No jobs available.");
        }
        else
        {
            /* Main iterate loop */
#ifdef FEATURE_TESTING
            cLOG(Level::INFO, "Iterate - Working with " 
                    + std::to_string(jobs_.size()) + " jobs ... ");
#endif // FEATURE_TESTING
            std::vector<opcuac::jobsptr> trash;
            
            for(auto job : jobs_)
            {
                auto local = std::dynamic_pointer_cast<open62541::Job>(job);
                if(local->get_info(PRAEFIX_TYPE) == "MitemAdd")
                {
                    add_monitored_item(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "MitemDel")
                {
                    delete_monitored_item(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "NodeRead")
                {
                    read_node(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "NodeWrite")
                {
                    write_node(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "Browse")
                {
                    browse_nodes(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "Print")
                {
                    print_label(job);
                }
                if(local->get_info(PRAEFIX_TYPE) == "Replace")
                {
                    replace_label(job);
                }
                
                /* Erase? */
                if(local->status() == STATUS_DEAD)
                {
                    trash.push_back(job);
                }
            }

            for(auto element : trash)
            {
                jobs_.remove(element);
            }

#ifdef FEATURE_TESTING
            cLOG(Level::INFO, "end iterate loop.");
#endif // FEATURE_TESTING
        }
    }
#else
    if(this->jobs_.empty())
    {
        return;
    }
    else
    {
        cLOG(Level::INFO, "Working with " 
                + std::to_string(jobs_.size()) + " jobs ... ");

        print_jobs_();

        std::vector<opcuac::jobsptr> trash;
        for(auto &job : jobs_)
        {
            job->status(STATUS_DEAD);
            /* Erase? */
            if(job->status() == STATUS_DEAD)
            {
                trash.push_back(job);
            }
        }
        for(auto element : trash)
        {
            jobs_.remove(element);
        }
    }
#endif // !FEATURE_CLIENT_OFF
    return;
}

void
Client::add_monitored_item(opcuac::jobsptr job)
{
    /* First add monitored item */
    if(job->status() == STATUS_ACTIVE)
    {
        jLOG(Level::JOB, "Add monitored item", job);

        /**
         * It is not possible to let the monitored item callback create the data object as 
         * its done in read_node method. Therefore the read_node method is used to attach 
         * a data packet of the correct type to the job.
        */
        client_->read_node(job);

        client_->add_monitored_item(job);

        job->status(STATUS_WORK);
    }
    /* First workbench to use the monitored data */
    /* No timestamps for data values are implemented */
    if(job->status() == STATUS_WORK)
    {
        dLOG(Level::DATA, "First space use monitored data. ", job);
    }
    return;
}

void
Client::delete_monitored_item(opcuac::jobsptr job)
{
    static int number = 0;
    /* The number is used to delay the deletion of the monitored item. */
    if(number++ == 25)
    {
        jLOG(Level::JOB, "Delete monitored item", job);

        client_->del_monitored_item(job);

        job->status(STATUS_DEAD);
    }

    return;
}

void 
Client::read_node(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Read node", job);

    client_->read_node(job);

    dLOG(Level::DATA, "Read node", job);
    
    job->status(STATUS_DEAD);
    return;
}

void
Client::write_node(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Write node", job);

    client_->write_node(job);

    job->status(STATUS_DEAD);
    return;
}

void
Client::browse_nodes(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Browse nodes with NodeTree objects", job);

    client_->browse(job);

    job->status(STATUS_DEAD);
    return;
}

void 
Client::print_label(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Print label with JScript/zpl", job);
//     if(job->intern_data->u16_value == 1)
//     {
//         /* Print */
//         /* Enter amount of labels */
//         job->intern_data->u16_value = 1;
//         opcuac_call_method_PrintCurrentLabel(job);
//     }
//     else if(job->intern_data->file_name.empty())
//     {
//         CAB_Client::upload_stream(job);
//     }
//     else
//     {
//         opcuac_call_method_FileUpload(job);
//         CAB_Client::upload_stream(job);
//     }
//     /* Erase from job list */
//     job->erase = true;
//     return;
}

void 
Client::replace_label(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Start replace items in label procedure", job);
//     /* Label with replace contents needs to load or print before */
//     if(job->intern_data->u32_value == 0)
//     {
//         
//         /* Need to know then contents item you want to replace */
//         std::shared_ptr<JOB> node = std::make_shared<JOB>(init_job());
//         for(auto it = job->intern_data->replace.begin(); 
//             it != job->intern_data->replace.end(); ++it)
//         {
//             /* Node settings */
//             node->type_id = id_type::string;
//             node->id_string = it->first;
//             node->namespace_index = 5;
//             /* Data settings */
//             node->intern_data->type_data = data_type::c_string;
//             node->intern_data->string_value = it->second;
            
//             if(opcuac_write_node(node) != EXIT_SUCCESS)
//             {
//                 cLOG(Level::INFO, "write replace contents failed.");
//                 job->erase = true;
//                 return;
//             }
//         }
//         job->intern_data->u32_value++;
//         return;
//     }
//     if(job->intern_data->u32_value == 1)
//     {
//         /* Also possible with print:JScript:PrintNow */
//         /* Amount of labels */
//         job->intern_data->u16_value = 1;
//         opcuac_call_method_PrintCurrentLabel(job);

//         /* Erase from job list */
//         job->erase = true;
//         return;
//     }
}

void
Client::show_usage(void)
{
    /* Usage */
    std::string file_path = DIR_RES + std::string("usage.txt");
    std::ifstream file(file_path);
    if(!file.is_open())
    {
        cLOG(Level::ERROR, "Can not open file; " + file_path);
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

bool
Client::file_finder(const std::string& dir, const std::string& file)
{
    std::string str;
    try 
    {
        if(dir == "current")
        {
            str = std::filesystem::current_path().string();
        }
        else
        {
            str = dir;
        }

        /* Check the directory */
        if (!std::filesystem::exists(str) || !std::filesystem::is_directory(str)) 
        {
            cLOG(Level::ERROR, "directory invalid: " + str);
            return false;
        }
        /* Iterate inside directory */
        for (const auto& entry : std::filesystem::directory_iterator(str)) 
        {
            if (entry.is_regular_file() && entry.path().filename() == file) 
            {
                return true;
            }
        }
    } 
    catch(const std::filesystem::filesystem_error& e) 
    {
        cLOG(Level::ERROR, "system directory error");
    }
    return false;
}

void
Client::create_job(const std::string& input)
{
    if(input.empty())
    {
        cLOG(Level::INFO, "input_string empty.");
        return;
    }

    /* Parse input in job arguments */
    std::map<int, std::string> input_map = parse_args(input);
    if(input_map.empty())
    {
        cLOG(Level::INFO, "input_map empty.");
        return;
    }

    auto item0 = input_map.begin();

    if(item0->second == "mitem_add")
    {
        auto joba = std::make_shared<open62541::Job>(input, "MitemAdd");
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);
        
        jobs_.push_back(jobb);
    }
    if(item0->second == "mitem_del")
    {
        auto joba = std::make_shared<open62541::Job>(input, "MitemDel");
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobs_.push_back(jobb);
    }
    if(item0->second == "node_read")
    {
        /* Read node */
        auto joba = std::make_shared<open62541::Job>(input, "NodeRead");
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobs_.push_back(jobb);
    }
    if(item0->second == "node_write")
    {
        /* Write node */
        auto joba = std::make_shared<open62541::Job>(input, "NodeWrite");
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobb->add_data(DATA_WRITE, init_data(input_map));
                
        jobs_.push_back(jobb);
    }
    if(item0->second == "browse")
    {
        /* Browse */
        auto joba = std::make_shared<open62541::Job>(input, "Browse");
        if(input_map.size() == 1)
        {
            auto jobb = nodeID_type(joba, std::to_string(UA_NS0ID_OBJECTSFOLDER), 
                    std::to_string(0));

            jobs_.push_back(jobb);
        }
        else
        {
            auto jobb = nodeID_type(joba, input_map.find(1)->second,
                    input_map.find(2)->second);

            jobs_.push_back(jobb);
        }
    }
    if(item0->second == "print")
    {
        auto joba = std::make_shared<open62541::Job>(input, "Print");
        // auto jobb = nodeID_type(joba, input_map.find(1)->second,
        //         input_map.find(2)->second);

    }
    if(item0->second == "replace")
    {
        // auto joba = std::make_shared<open62541::Job>(input, "Replace");
        // auto jobb = nodeID_type(joba, input_map.find(1)->second,
        //         input_map.find(2)->second);

    }

    // jsptr job = std::make_shared<Job>();

    // job->init_string = input_string;
    
    // auto element0 = *input_map.begin();
    // job->type = get_job_type(element0.second);

    // /* Node id, type_id and namespace settings */
    // if(!(job->type == job_type::print || 
    //      job->type == job_type::replace))
    // {
    //     if(input_map.size() > 1)
    //     {
    //         /* Id and type_id*/
    //         auto element1 = *input_map.find(1);
    //         if(CAB_Client::request_all_digits(element1.second))
    //         {
    //             job->id_numeric = std::stoi(element1.second);
    //             job->type_id = id_type::numeric;
    //         }
    //         else
    //         {
    //             job->id_string = element1.second;
    //             job->type_id = id_type::string;
    //         }

    //         /* Namespace */
    //         auto element2 = *input_map.find(2);
    //         job->namespace_index = std::stoi(element2.second);
    //     }
    // }
    
    
    // else if(job->type == job_type::print)
    // {
    //     /* Find language type */
    //     job->intern_data->type_data = data_type::c_boolean;
    //     auto element1 = *input_map.find(1);
    //     if(element1.second == "zpl")
    //     {
    //         element1.second = "true";
    //     }
    //     else
    //     {
    //         element1.second = "false";
    //     }
    //     set_data_value(element1.second, job);
    
    //     /* Find label content without validation */
    //     job->intern_data->type_data = data_type::c_string;
    //     auto element2 = *input_map.find(2);
    //     if(find_file_in_directory(DIR_RES, element2.second))
    //     {
    //         std::string temp_path = DIR_RES + element2.second;

    //         cLOG(Level::INFO, "Use label content from file " + temp_path);
    //         std::ifstream file(temp_path);
    //         if(file.is_open())
    //         {
    //             std::stringstream string_stream;
    //             string_stream << file.rdbuf();
    //             job->intern_data->string_value = string_stream.str() + "\n";
    //             file.close();
    //         }
    //         else
    //         {
    //             cLOG(Level::ERROR, "Can not open file " + temp_path);
    //         }
    //     }
    //     else if(element2.second == "PrintNow")
    //     {
    //         job->intern_data->u16_value = 1;
    //     }
    //     else
    //     {
    //         job->intern_data->string_value = element2.second + "\n";
    //     }
    //     /* Find bitmap or more JScript */
    //     if(input_map.size() > 3)
    //     {
    //         auto element3 = *input_map.find(3);
    //         if(find_file_in_directory(DIR_RES, element3.second) == true)
    //         {
    //             job->intern_data->file_name = DIR_RES + element3.second;
    //         }
    //         else
    //         {
    //             std::string str;
    //             for(auto it = input_map.find(2); it != input_map.end(); ++it)
    //             {
    //                 if(!str.empty())
    //                 {
    //                     str += ":";
    //                 }
    //                 str += it->second;
    //             }
    //             job->intern_data->string_value = str + "\n";
    //         }
    //     }
    // }
    // else if(job->type == job_type::replace)
    // {
    //     /* Get data for replacement */
    //     if(input_map.size() < 3 || !(input_map.size() % 2))
    //     {
    //         cLOG(Level::ERROR, "you entered replace with invalid arguments.");
    //         return;
    //     }
    //     /* Parse replacement data */
    //     std::map<std::string, std::string> replacements;
    //     for(auto it = input_map.find(1); it != input_map.end(); ++it)
    //     {
    //         std::string dummy = it->second;
    //         if (++it == input_map.end()) 
    //         {
    //             break;
    //         }
    //         replacements.emplace(dummy, it->second);
    //     }
    //     job->intern_data->replace = replacements;
    // }
    // else if(job->type == job_type::DEFAULT)
    // {
    //     cLOG(Level::INFO, "job_type is invalid. No job was create.");
    //     return;
    // }

    // /* Use extern DATA space? */
    // if(shared_data != nullptr)
    // {
    //     job->intern_data = shared_data;
    //     new_jobs_.push_back(job);
    //     return;
    // }
    
    // /* Push back actual job configuration */
    // job_buffer_.push_back(job);
    return;
} 

std::map<int, std::string>
Client::parse_args(const std::string& input)
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
        cLOG(Level::ERROR, "syntax error.");
        return result;
    }

    std::stringstream ss(input);
    std::string item;
    int index = 0;
    while(std::getline(ss, item, ':'))
    {
        if(item.empty())
        {
            cLOG(Level::ERROR, "single argument is empty.");
            std::map<int, std::string> end;
            return end;
        }
        result.emplace(index++, item);
    }    
    return result;
}

bool 
Client::digits(const std::string& str) 
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

opcuac::jobsptr
Client::nodeID_type(opcuac::jobsptr job, const std::string& id, const std::string& ns)
{
    if(digits(id))
    {
        return std::make_shared<open62541::JNumeric>(job, std::stoi(id), std::stoi(ns));
    }
    else
    {
        return std::make_shared<open62541::JString>(job, id, std::stoi(ns));
    }
}

void
Client::print_jobs_(void)
{
    if(jobs_.empty() == true)
    {
        cLOG(Level::INFO, "Job list empty.");
        return;
    }

    cLOG(Level::INFO, "Print job list.\n" + CONSOLE_LINE_50);
    for(auto dummy : jobs_)
    {
        print_job(dummy);
        std::cout << CONSOLE_LINE_50 << std::endl;
    }
    return;
}

void
Client::print_job(opcuac::jobsptr job)
{
    auto local = dynamic_cast<open62541::Job*>(job.get());

    if(local == nullptr)
    {
        cLOG(Level::INFO, "Job is nullptr!");
        return;
    }
    else
    {
        std::cout << *local;
        return;
    }
}

opcuac::datasptr 
Client::init_data(std::map<int, std::string>& input) const
{
    /* Check size */
    if(input.size() < 5)
        throw std::runtime_error("Not enough values in job init_string.");

    std::string type = input.find(3)->second;
    if(type == "opc_time_t")
    {
        if(input.find(4)->second == "actual")
        {
            return std::make_shared<open62541::DDateTime>(system_time());
        }
        else if(input.size() < 10)
            throw std::runtime_error("Not enough values in job init_string.");
        else
        {
            opc_time_t time;
            time.day = std::stoi(input.find(4)->second);
            time.mon = std::stoi(input.find(5)->second);
            time.year = std::stoi(input.find(6)->second);
            time.hour = std::stoi(input.find(7)->second);
            time.min = std::stoi(input.find(8)->second);
            time.sec = std::stoi(input.find(9)->second);
            time.msec = std::stoi(input.find(10)->second);

            return std::make_shared<open62541::DDateTime>(time);
        }
    }
    if(type == "bool_t")
    {
        return std::make_shared<open62541::DBoolean>(std::stoi(input.find(4)->second));
    }
    if(type == "int16_t")
    {
        return std::make_shared<open62541::DInt16>(std::stoi(input.find(4)->second));
    }
    if(type == "uint16_t")
    {
        int dummy = std::stoi(input.find(4)->second);
        if(dummy < 0)
            throw std::out_of_range("Negative value detected for uint16_t");
        else
        {
            return std::make_shared<open62541::DUInt16>(dummy);
        }
    }
    if(type == "int32_t")
    {
        return std::make_shared<open62541::DInt32>(std::stoi(input.find(4)->second));
    }
    if(type == "uint32_t")
    {
        int dummy = std::stoi(input.find(4)->second);
        if(dummy < 0)
            throw std::out_of_range("Negative value detected for uint32_t");
        else
        {
            return std::make_shared<open62541::DUInt16>(dummy);
        }
    }
    if(type == "int64_t")
    {
        return std::make_shared<open62541::DInt64>(std::stoi(input.find(4)->second));
    }
    if(type == "uint64_t")
    {
        int dummy = std::stoi(input.find(4)->second);
        if(dummy < 0)
            throw std::out_of_range("Negative value detected for uint64_t");
        else
        {
            return std::make_shared<open62541::DUInt16>(dummy);
        }
    }
    if(type == "f32_t")
    {
        return std::make_shared<open62541::DFloat>(std::stoi(input.find(4)->second));
    }
    if(type == "f64_t")
    {
        return std::make_shared<open62541::DDouble>(std::stoi(input.find(4)->second));
    }
    if(type == "string")
    {
        return std::make_shared<open62541::DString>(input.find(4)->second);
    }

    throw std::invalid_argument("Invalid data type specified");
}

// opcuac::datasptr
// Client::init_data(std::string type) const 
// {
//     if(type == "opc_time_t")
//     {
//         opc_time_t time;
//         time.day = 0;
//         time.mon = 0;
//         time.year = 0;
//         time.hour = 0;
//         time.min = 0;
//         time.sec = 0;
//         time.msec = 0;
//         return std::make_shared<open62541::DDateTime>(time);
//     }
//     if(type == "bool_t")
//     {
//         return std::make_shared<open62541::DBoolean>(0);
//     }
//     if(type == "int16_t")
//     {
//         return std::make_shared<open62541::DInt16>(0);
//     }
//     if(type == "uint16_t")
//     {
//         return std::make_shared<open62541::DUInt16>(0);
//     }
//     if(type == "int32_t")
//     {
//         return std::make_shared<open62541::DInt32>(0);
//     }
//     if(type == "uint32_t")
//     {
//         return std::make_shared<open62541::DUInt16>(0);
//     }
//     if(type == "int64_t")
//     {
//         return std::make_shared<open62541::DInt64>(0);
//     }
//     if(type == "uint64_t")
//     {
//         return std::make_shared<open62541::DUInt16>(0);
//     }
//     if(type == "f32_t")
//     {
//         return std::make_shared<open62541::DFloat>(0);
//     }
//     if(type == "f64_t")
//     {
//         return std::make_shared<open62541::DDouble>(0);
//     }
//     if(type == "string")
//     {
//         return std::make_shared<open62541::DString>(0);
//     }

//     throw std::invalid_argument("Invalid data type specified");
// }

opc_time_t
Client::system_time(void) const
{
    auto now = std::chrono::system_clock::now();
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    time_t time = std::chrono::system_clock::to_time_t(now);
    struct tm time_now;

    oswrapper::localtime(&time, &time_now);

    opc_time_t actual;
    actual.msec = static_cast<int>(msec.count() % 1000);
    actual.sec = time_now.tm_sec;
    actual.min = time_now.tm_min;
    actual.hour = time_now.tm_hour;
    actual.day = time_now.tm_mday;
    actual.mon = time_now.tm_mon + 1;
    actual.year = time_now.tm_year + 1900;

    cLOG(Level::INFO, "Time: "
        + std::to_string(actual.day) + '.'
        + std::to_string(actual.mon) + '.'
        + std::to_string(actual.year) + " | "
        + std::to_string(actual.hour) + ':'
        + std::to_string(actual.min) + ':'
        + std::to_string(actual.sec) + ':'
        + std::to_string(actual.msec));

    return actual;
}

// data_time Client::get_system_time(void) 
// {
//     auto now = std::chrono::system_clock::now();
//     auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

//     time_t t = std::chrono::system_clock::to_time_t(now);
//     struct tm now_tm;

//     lset::wrapper_localtime(&t, &now_tm);

//     data_time dt;
//     dt.tm_msec = static_cast<int>(milliseconds.count() % 1000);
//     dt.tm_sec = now_tm.tm_sec;
//     dt.tm_min = now_tm.tm_min;
//     dt.tm_hour = now_tm.tm_hour;
//     dt.tm_mday = now_tm.tm_mday;
//     dt.tm_mon = now_tm.tm_mon + 1;
//     dt.tm_year = now_tm.tm_year + 1900;

//     cLOG(Level::INFO, "Time: "
//             + std::to_string(dt.tm_mday) + '.'
//             + std::to_string(dt.tm_mon) + '.'
//             + std::to_string(dt.tm_year) + " | "
//             + std::to_string(dt.tm_hour) + ':'
//             + std::to_string(dt.tm_min) + ':'
//             + std::to_string(dt.tm_sec) + ':'
//             + std::to_string(dt.tm_msec));
//     return dt;
// }


// void
// Client::upload_stream(std::shared_ptr<JOB> job)
// {
//     /* Guarantees that labels or files has been uploaded completely */
//     /* Check upload status - Current Label Definition Completion Sequence */
//     // print_info(job->intern_data->string_value);
//     /* Depends on job syntax, no handling for zpl implemented */
//     /* feed? */
//     if(job->intern_data->string_value == "f\n")
//     {
//         /* Method call without monitoring */
//         opcuac_call_method_PrintData(job);
//         return;
//     }
//     else
//     {
//         std::shared_ptr<JOB> node = std::make_shared<JOB>(init_job());
//         node->type_id = id_type::numeric;
//         node->id_numeric = 10086;
//         node->namespace_index = 3;

//         opcuac_read_node(node);
//         int start = node->intern_data->u32_value;
//         int end = start;
//         cLOG(Level::INFO, "start counter: " + start);

//         /* Monitored method call */
//         opcuac_call_method_PrintData(job);
//         int breaker = 0;
//         do
//         {
//             breaker++;
//             opcuac_read_node(node);
//             end = node->intern_data->u32_value;
//             cLOG(Level::INFO, "end counter: " + std::to_string(end) + 
//                 " - breaker = " + std::to_string(breaker) + "/100");
//         } while (end == start && breaker < 100);
//         if(breaker == 100)
//         {
//             cLOG(Level::ERROR, "breaker stopped loop");
//         }
//         return;
//     }
// }







// job_type
// Client::get_job_type(std::string &string)
// {
//     job_type job = job_type::DEFAULT;
//     if      (string == "mitem_add") return job = job_type::mitem_add;
//     else if (string == "mitem_delete") return job = job_type::mitem_delete;
//     else if (string == "node_read") return job = job_type::node_read;
//     else if (string == "node_write") return job = job_type::node_write;
//     else if (string == "browse") return job = job_type::browse;
//     else if (string == "print") return job = job_type::print;
//     else if (string == "replace") return job = job_type::replace;
//     else return job = job_type::DEFAULT;
// }

// job_type
// Client::get_job_type(std::shared_ptr<JOB> job)
// {
//     return job->type;
// }

// std::string 
// Client::get_job_type_string(const job_type &type)
// {
//     if      (type == job_type::mitem_add) return "mitem_add";
//     else if (type == job_type::mitem_delete) return "mitem_delete";
//     else if (type == job_type::node_read) return "node_read";
//     else if (type == job_type::node_write) return "node_write";
//     else if (type == job_type::browse) return "browse";
//     else if (type == job_type::print) return "print";
//     else if (type == job_type::replace) return "replace";
//     else return "DEFAULT";
// }

// std::string
// Client::get_id_type_string(const id_type &type)
// {
//     if      (type == id_type::string) return "string";
//     else if (type == id_type::numeric) return "numeric";
//     else return "DEFAULT";
// }

// std::string
// Client::get_data_type_string(const data_type &type)
// {
//     if      (type == data_type::c_datetime) return "datetime";
//     else if (type == data_type::c_boolean) return "bool";
//     else if (type == data_type::c_int16) return "int16_t";
//     else if (type == data_type::c_uint16) return "uint16_t";
//     else if (type == data_type::c_int32) return "int32_t";
//     else if (type == data_type::c_uint32) return "uint32_t";
//     else if (type == data_type::c_int64) return "int64_t";
//     else if (type == data_type::c_uint64) return "uint64_t";
//     else if (type == data_type::c_f32) return "f32_t";
//     else if (type == data_type::c_f64) return "f64_t";
//     else if (type == data_type::c_string) return "string";
//     else if (type == data_type::DEFAULT) return "DEFAULT";
//     else return "DEFAULT";
// }

// data_type
// Client::get_data_type(const std::string &data_type)
// {
//     if      (data_type == "datetime") return data_type::c_datetime;
//     else if (data_type == "bool") return data_type::c_boolean;
//     else if (data_type == "int16_t") return data_type::c_int16;
//     else if (data_type == "uint16_t") return data_type::c_uint16;
//     else if (data_type == "int32_t") return data_type::c_int32;
//     else if (data_type == "uint32_t") return data_type::c_uint32;
//     else if (data_type == "int64_t") return data_type::c_int64;
//     else if (data_type == "uint64_t") return data_type::c_uint64;
//     else if (data_type == "f32_t") return data_type::c_f32;
//     else if (data_type == "f64_t") return data_type::c_f64;
//     else if (data_type == "string") return data_type::c_string;
//     else if (data_type == "DEFAULT") return data_type::DEFAULT;
//     else return data_type::DEFAULT;
// }

// int
// Client::int_from_string(std::string &string)
// {
//     // std::cout << string << " | " << string.size() << std::endl;
//     if(request_all_digits(string) == true)
//     {
//         return std::stoi(string);
//     }
//     return - 1;
// }

// std::string
// Client::get_url(void)
// {
//     return CAB_Client::client_url_;
// }

// void
// Client::set_url(std::string &url)
// {
//     CAB_Client::client_url_ = url;
//     return;
// }

// void
// Client::set_data_value(const std::string &data_value,
//                            std::shared_ptr<JOB> &job)
// {
//     switch (job->intern_data->type_data)
//     {
//     case data_type::c_datetime:
//         job->intern_data->time = string_data_time(data_value);
//         break;
//     case data_type::c_boolean:
//         job->intern_data->b_value = string_bool(data_value);
//         break;
//     case data_type::c_int16:
//         job->intern_data->s16_value = string_int16_t(data_value);
//         break;
//     case data_type::c_uint16:
//         job->intern_data->u16_value = string_uint16_t(data_value);
//         break;
//     case data_type::c_int32:
//         job->intern_data->s32_value = string_int32_t(data_value);
//         break;
//     case data_type::c_uint32:
//         job->intern_data->u32_value = string_uint32_t(data_value);
//         break;
//     case data_type::c_int64:
//         job->intern_data->s64_value = string_int64_t(data_value);
//         break;
//     case data_type::c_uint64:
//         job->intern_data->u64_value = string_uint64_t(data_value);
//         break;
//     case data_type::c_f32:
//         job->intern_data->f32_value = string_f32_t(data_value);
//         break;
//     case data_type::c_f64:
//         job->intern_data->f64_value = string_f64_t(data_value);
//         break;
//     case data_type::c_string:
//         job->intern_data->string_value = data_value;
//         break;
//     case data_type::DEFAULT:
//         break;
//     default:
//         break;
//     }
//     return;
// }

// std::string
// Client::get_data_value_string(std::shared_ptr<JOB> &job)
// {
//     std::string time_as_string;
//     std::ostringstream oss;
//     switch (job->intern_data->type_data)
//     {
//     case data_type::c_datetime:
//         oss << std::setfill('0')
//             << std::setw(2) << job->intern_data->time.tm_mday << '.'
//             << std::setw(2) << job->intern_data->time.tm_mon + 1 << '.'
//             << std::setw(4) << job->intern_data->time.tm_year + 1900 << " | "
//             << std::setw(2) << job->intern_data->time.tm_hour << ':'
//             << std::setw(2) << job->intern_data->time.tm_min << ':'
//             << std::setw(2) << job->intern_data->time.tm_sec << ':'
//             << std::setw(3) << job->intern_data->time.tm_msec;
//         time_as_string = oss.str();
//         return time_as_string;      
//         break;
//     case data_type::c_boolean:
//         return std::to_string(job->intern_data->b_value);
//         break;
//     case data_type::c_int16:
//         return std::to_string(job->intern_data->s16_value);
//         break;
//     case data_type::c_uint16:
//         return std::to_string(job->intern_data->u16_value);
//         break;
//     case data_type::c_int32:
//         return std::to_string(job->intern_data->s32_value);
//         break;
//     case data_type::c_uint32:
//         return std::to_string(job->intern_data->u32_value);
//         break;
//     case data_type::c_int64:
//         return std::to_string(job->intern_data->s64_value);
//         break;
//     case data_type::c_uint64:
//         return std::to_string(job->intern_data->u64_value);
//         break;
//     case data_type::c_f32:
//         return std::to_string(job->intern_data->f32_value);
//         break;
//     case data_type::c_f64:
//         return std::to_string(job->intern_data->f64_value);
//         break;
//     case data_type::c_string:
//         return job->intern_data->string_value;
//         break;
//     case data_type::DEFAULT:
//         return "DEFAULT";
//         break;
//     default:
//         break;
//     }
//     return "DEFAULT";
// }



// void
// Client::print_job(std::shared_ptr<JOB> &job)
// {
//     if(get_job_type(job) == job_type::node_write)
//     {
//         std::cout   << std::setw(20) << std::left << "| type: " 
//                     << std::setw(59) << std::left << get_job_type_string(job->type) << '|' << std::endl
//                     << std::setw(20) << std::left << "| type_id: " 
//                     << std::setw(59) << std::left << get_id_type_string(job->type_id) << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_string: " 
//                     << std::setw(59) << std::left << job->id_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_numeric: " 
//                     << std::setw(59) << std::left << job->id_numeric << '|' << std::endl
//                     << std::setw(20) << std::left << "| ns: " 
//                     << std::setw(59) << std::left << job->namespace_index << '|' << std::endl
//                     << std::setw(20) << std::left << "| init_string: " 
//                     << std::setw(59) << std::left << job->init_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| data_type: " 
//                     << std::setw(59) << std::left << get_data_type_string(job->intern_data->type_data) << '|' << std::endl
//                     << std::setw(20) << std::left << "| data_value: " 
//                     << std::setw(59) << std::left << get_data_value_string(job) << '|' << std::endl
//                     << CONSOLE_LINE_80 
//                     << std::endl;
//     }
//     else if(get_job_type(job) == job_type::print)
//     {
//         std::string sub_string = {""};
//         if(get_data_value_string(job).length() > 50)
//         {
//             sub_string = get_data_value_string(job).substr(0, 50) + "\n ...";
//         }
//         else
//         {
//             sub_string = get_data_value_string(job) + " ...";
//         }
//         std::cout   << std::setw(20) << std::left << "| type: " 
//                     << std::setw(59) << std::left << get_job_type_string(job->type) << '|' << std::endl
//                     << std::setw(20) << std::left << "| type_id: " 
//                     << std::setw(59) << std::left << get_id_type_string(job->type_id) << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_string: " 
//                     << std::setw(59) << std::left << job->id_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_numeric: " 
//                     << std::setw(59) << std::left << job->id_numeric << '|' << std::endl
//                     << std::setw(20) << std::left << "| ns: " 
//                     << std::setw(59) << std::left << job->namespace_index << '|' << std::endl
//                     << std::setw(20) << std::left << "| init_string: " 
//                     << std::setw(59) << std::left << job->init_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| data_type: " 
//                     << std::setw(59) << std::left << get_data_type_string(job->intern_data->type_data) << '|' << std::endl
//                     << std::setw(20) << std::left << "| data_value: \n" 
//                     << std::setw(59) << std::left << sub_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| zpl: " 
//                     << std::setw(59) << std::left << job->intern_data->b_value << '|' << std::endl
//                     << std::setw(20) << std::left << "| bitmap: " 
//                     << std::setw(59) << std::left << job->intern_data->file_name << '|' << std::endl
//                     << CONSOLE_LINE_80 
//                     << std::endl;
//     }
//     else if(get_job_type(job) == job_type::replace)
//     {
//         std::cout   << std::setw(20) << std::left << "| type: " 
//                     << std::setw(59) << std::left << get_job_type_string(job->type) << '|' << std::endl
//                     << std::setw(20) << std::left << "| type_id: " 
//                     << std::setw(59) << std::left << get_id_type_string(job->type_id) << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_string: " 
//                     << std::setw(59) << std::left << job->id_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_numeric: " 
//                     << std::setw(59) << std::left << job->id_numeric << '|' << std::endl
//                     << std::setw(20) << std::left << "| ns: " 
//                     << std::setw(59) << std::left << job->namespace_index << '|' << std::endl
//                     << std::setw(20) << std::left << "| init_string: " 
//                     << std::setw(59) << std::left << job->init_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| data_value: " 
//                     << std::setw(59) << std::left << '|' << std::endl;
//         for(auto it = job->intern_data->replace.begin(); it != job->intern_data->replace.end(); ++it)
//         {
//             std::cout   << std::setw(20) << std::left << "| Replace " 
//                         << it->first 
//                         << " contents with " 
//                         << FONT_LIGHT_PURPLE
//                         << it->second 
//                         << FONT_RESET
//                         << '|' << std::endl;
//         }
//         std::cout   << CONSOLE_LINE_80 
//                     << std::endl;            

                    
//     }
//     else
//     {
//         std::cout   << std::setw(20) << std::left << "| type: " 
//                     << std::setw(59) << std::left << get_job_type_string(job->type) << '|' << std::endl
//                     << std::setw(20) << std::left << "| type_id: " 
//                     << std::setw(59) << std::left << get_id_type_string(job->type_id) << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_string: " 
//                     << std::setw(59) << std::left << job->id_string << '|' << std::endl
//                     << std::setw(20) << std::left << "| id_numeric: " 
//                     << std::setw(59) << std::left << job->id_numeric << '|' << std::endl
//                     << std::setw(20) << std::left << "| ns: " 
//                     << std::setw(59) << std::left << job->namespace_index << '|' << std::endl
//                     << std::setw(20) << std::left << "| init_string: " 
//                     << std::setw(59) << std::left << job->init_string << '|' << std::endl
//                     << CONSOLE_LINE_80 
//                     << std::endl;
//     }
//     return;
// }

// void
// Client::print_console_message_data(std::shared_ptr<JOB> &job, 
//                         std::string type)
// {
//     std::cout << FONT_LIGHT_BLUE;
//     switch (job->intern_data->type_data)
//     {
//     case data_type::c_datetime:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     /* Date */
//                     << std::setfill('0') 
//                     << std::setw(2) << job->intern_data->time.tm_mday << "."
//                     << std::setw(2) << job->intern_data->time.tm_mon + 1 << "."
//                     << std::setw(4) << job->intern_data->time.tm_year + 1900 << " | "
//                     /* Time */
//                     << std::setw(2) << job->intern_data->time.tm_hour << ":"
//                     << std::setw(2) << job->intern_data->time.tm_min << ":"
//                     << std::setw(2) << job->intern_data->time.tm_sec << ":"
//                     << std::setw(3) << job->intern_data->time.tm_msec
//                     << std::endl;
//         break;
//     case data_type::c_boolean:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->b_value
//                     << std::endl;
//         break;
//     case data_type::c_int16:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->s16_value
//                     << std::endl;
//         break;
//     case data_type::c_uint16:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->u16_value
//                     << std::endl;
//         break;
//     case data_type::c_int32:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->s32_value
//                     << std::endl;
//         break;
//     case data_type::c_uint32:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - " 
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->u32_value
//                     << std::endl;
//         break;
//     case data_type::c_int64:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->s64_value
//                     << std::endl;
//         break;
//     case data_type::c_uint64:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->u64_value
//                     << std::endl;
//         break;
//     case data_type::c_f32:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->f32_value
//                     << std::endl;
//         break;
//     case data_type::c_f64:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->f64_value
//                     << std::endl;
//         break;
//     case data_type::c_string:
//         std::cout << CAB_USER
//                     << " - " 
//                     << job->init_string << " - "
//                     << type << ": "
//                     << "(" << get_data_type_string(job->intern_data->type_data) << ") "
//                     /* Data */
//                     << job->intern_data->string_value
//                     << std::endl;
//         break;
//     case data_type::DEFAULT:
//         break;
//     default:
//         break;
//     }
//     std::cout << FONT_RESET;
//     return;
// }

// data_time
// Client::string_data_time(const std::string& string)
// {
//     data_time time = get_system_time();
    
//     if(string == "actual")
//     {
//         return time; 
//     }
//     else
//     {
//         std::map<int, std::string> input = parse_arguments(string);
//         if(input.size() != 7)
//         {
//             cLOG(Level::INFO, "argument syntax invalid. return actual time.");
//             return time;
//         }

//         /* Input Syntax day:month:year:hour:min:sec:msec */
//         auto e1 = *input.find(0);
//         auto e2 = *input.find(1);
//         auto e3 = *input.find(2);
//         auto e4 = *input.find(3);
//         auto e5 = *input.find(4);
//         auto e6 = *input.find(5);
//         auto e7 = *input.find(6);

//         /* No data validation */
//         time.tm_mday = std::stoi(e1.second);
//         time.tm_mon = std::stoi(e2.second) - 1;
//         time.tm_year = std::stoi(e3.second) - 1900;
//         time.tm_hour = std::stoi(e4.second);
//         time.tm_min = std::stoi(e5.second);
//         time.tm_sec = std::stoi(e6.second);
//         time.tm_msec = std::stoi(e7.second);

//         cLOG(Level::INFO, "Time input: "
//                 + std::to_string(time.tm_mday) + '.'
//                 + std::to_string(time.tm_mon) + '.'
//                 + std::to_string(time.tm_year) + " | "
//                 + std::to_string(time.tm_hour) + ':'
//                 + std::to_string(time.tm_min) + ':'
//                 + std::to_string(time.tm_sec) + ':'
//                 + std::to_string(time.tm_msec));

//         return time;
//     }
// }



//data_time 
//Client::get_system_time(void) 
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
//Client::get_system_time(void) 
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

// bool 
// Client::string_bool(const std::string& string)
// {
//     if(string == "true" || string == "1")
//     {
//         return true;
//     }
//     else if(string == "false" || string == "0")
//     {
//         return false;
//     }
//     else
//     {
//         return false;
//     }
// }

// int16_t
// Client::string_int16_t(const std::string& string)
// {
//     return static_cast<int16_t>(std::stoi(string));
// }

// uint16_t
// Client::string_uint16_t(const std::string& string)
// {
//     return static_cast<uint16_t>(std::stoul(string));
// }

// int32_t 
// Client::string_int32_t(const std::string& string)
// {
//     return static_cast<int32_t>(std::stoi(string));
// }

// uint32_t 
// Client::string_uint32_t(const std::string& string)
// {
//     return static_cast<uint32_t>(std::stoul(string));
// }

// int64_t
// Client::string_int64_t(const std::string& string)
// {
//     return static_cast<int64_t>(std::stoi(string));
// }

// uint64_t
// Client::string_uint64_t(const std::string& string)
// {
//     return static_cast<uint64_t>(std::stoul(string));
// }

// f32_t
// Client::string_f32_t(const std::string& string)
// {
//     return std::stof(string);
// }

// f64_t
// Client::string_f64_t(const std::string& string)
// {
//     return std::stod(string);
// }








/* Eof */