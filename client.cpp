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
            cLOG(Level::INFO, "End iterate loop.");
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

    if(job->get_info(PRAEFIX_JOBTYPE) == "FileUpload")
    {
        cLOG(Level::INFO, "Upload your insert file, bitmap or picture.");

        /* Finish upload before working with other methods */
        client_->file_upload(job);

        job->status(STATUS_DEAD);
        return;
    }

    if(job->get_info(PRAEFIX_JOBTYPE) == "PrintNow")
    {
        cLOG(Level::INFO, "Print current label.");

        client_->print_current_label(job);

        job->status(STATUS_DEAD);
        return;
    }

    cLOG(Level::INFO, "Print contents from job input or file.");

    Client::upload_stream(job);

    job->status(STATUS_DEAD);
    return;
}

void 
Client::replace_label(opcuac::jobsptr job)
{
    jLOG(Level::JOB, "Start replace items in label procedure", job);
    
    client_->write_node(job);

    job->status(STATUS_DEAD);
    return; 
}

void
Client::upload_stream(opcuac::jobsptr job)
{
    /* Guarantees that labels or files has been uploaded completely */
    /* Check upload status - Current Label Definition Completion Sequence */
    /* Depends on job syntax, no handling for zpl implemented */
    /* Current Label Definition Completion Sequence = CLDCS */
    auto joba = std::make_shared<open62541::Job>("CLDCS", JOB_NODEREAD);
    auto jobb = nodeID_type(joba, "10086", "3");
    
    /* feed? */
    auto print = job->get_data(DATA_UPLOAD);
    if(print != nullptr)
    {
        auto value = std::dynamic_pointer_cast<open62541::DString>(print);
        if(value->get() == "f\n")
        {
            /* Method call without monitoring in case feed */
            client_->file_upload(job);
            
            return;
        }
        else
        {
            client_->read_node(jobb);
            auto data = jobb->get_data(DATA_READ);
            auto value = std::static_pointer_cast<open62541::DUInt32>(data);
            int start = value->get();
            int end = start;

            cLOG(Level::INFO, "Start data upload.");
            /* Method call print data */
            client_->print_data(job);

            int breaker = 0;
            do
            {
                breaker++;
                client_->read_node(jobb);
                auto tempd = jobb->get_data(DATA_READ);
                auto tempv = std::static_pointer_cast<open62541::DUInt32>(tempd);
                end = tempv->get();
            } while (end == start && breaker < 100);

            if(breaker == 100)
            {
                cLOG(Level::ERROR, "Data upload stopped! Check code!");
                return;
            }
            cLOG(Level::INFO, "Finish data upload.");
            return;
        }
    }
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
            cLOG(Level::ERROR, "Directory invalid: " + str);
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
        cLOG(Level::ERROR, "System directory error");
    }
    return false;
}

void
Client::create_job(const std::string& input)
{
    if(input.empty())
    {
        cLOG(Level::INFO, "Input_string empty.");
        return;
    }

    /* Parse input in job arguments */
    std::map<int, std::string> input_map = parse_args(input);
    if(input_map.empty())
    {
        cLOG(Level::INFO, "Input_map empty.");
        return;
    }

    auto item0 = input_map.begin();

    if(item0->second == "mitem_add")
    {
        auto joba = std::make_shared<open62541::Job>(input, JOB_MITEMADD);
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);
        
        jobs_.push_back(jobb);
        return;
    }
    if(item0->second == "mitem_del")
    {
        auto joba = std::make_shared<open62541::Job>(input, JOB_MITEMDEL);
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobs_.push_back(jobb);
        return;
    }
    if(item0->second == "node_read")
    {
        /* Read node */
        auto joba = std::make_shared<open62541::Job>(input, JOB_NODEREAD);
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobs_.push_back(jobb);
        return;
    }
    if(item0->second == "node_write")
    {
        /* Write node */
        auto joba = std::make_shared<open62541::Job>(input, JOB_NODEWRITE);
        auto jobb = nodeID_type(joba, input_map.find(1)->second,
                input_map.find(2)->second);

        jobb->add_data(DATA_WRITE, init_data(input_map));
                
        jobs_.push_back(jobb);
        return;
    }
    if(item0->second == "browse")
    {
        /* Browse */
        auto joba = std::make_shared<open62541::Job>(input, JOB_BROWSE);
        if(input_map.size() == 1)
        {
            auto jobb = nodeID_type(joba, std::to_string(UA_NS0ID_OBJECTSFOLDER), 
                    std::to_string(0));

            jobs_.push_back(jobb);
            return;
        }
        else
        {
            auto jobb = nodeID_type(joba, input_map.find(1)->second,
                    input_map.find(2)->second);

            jobs_.push_back(jobb);
            return;
        }
    }
    if(item0->second == "print")
    {
        auto joba = std::make_shared<open62541::Job>(input, JOB_PRINT);
        /* Set printer language */
        if(input_map.find(1)->second == "JScript")
        {
            joba->add_info(PRAEFIX_LANGUAGE, "js");
        }
        else
        {
            joba->add_info(PRAEFIX_LANGUAGE, "zpl");
        }
        /* Data upload used */
        if(input_map.size() == 4 && file_finder(DIR_RES, input_map.find(3)->second))
        {
            /* Extra File : ns = 3 : id = 20006 */
            auto jobb = nodeID_type(joba, "20006", "3");
            jobb->add_info(PRAEFIX_JOBTYPE, "FileUpload");
            /* Data */
            jobb->add_data(DATA_UPLOAD,
                    std::make_shared<open62541::DString>(DIR_RES + input_map.find(3)->second));

            jobs_.push_front(jobb);
        }
        /* Contents as file or string */
        if(input_map.find(2)->second == "PrintNow")
        {
            /* Print current label : ns = 4 : id = 10098 */
            auto jobb = nodeID_type(joba, "10098", "3");
            jobb->add_info(PRAEFIX_JOBTYPE, "PrintNow");
            /* Nothing to do */

            jobs_.push_back(jobb);
            return;
        }
        if(file_finder(DIR_RES, input_map.find(2)->second) == true)
        {
            /* File : ns = 3 : id = 6008 */
            auto jobb = nodeID_type(joba, "6008", "3");
            jobb->add_info(PRAEFIX_JOBTYPE, "ContentsFile");
            /* Data */
            std::string temp_path = DIR_RES + input_map.find(2)->second;
            cLOG(Level::INFO, "Use label content from file " + temp_path);
            std::ifstream file(temp_path);
            if(file.is_open())
            {
                std::stringstream string_stream;
                string_stream << file.rdbuf();
                file.close();

                jobb->add_data(DATA_UPLOAD, 
                        std::make_shared<open62541::DString>(string_stream.str() + "\n"));

                jobs_.push_back(jobb);
                return;
            }
            else
            {
                cLOG(Level::ERROR, "Can not open file " + temp_path);
                return;
            }
        }
        else
        {
            /* Contents : ns = 3 : id = 6008 */
            auto jobb = nodeID_type(joba, "6008", "3");
            jobb->add_info(PRAEFIX_JOBTYPE, "ContentsInput");
            /* Data */
            jobb->add_data(DATA_PRINT,
                    std::make_shared<open62541::DString>(input_map.find(2)->second + "\n"));

            jobs_.push_back(jobb);
        }
    }
    if(item0->second == "replace")
    {
        /* Get data for replacement */
        if(input_map.size() < 3 || !(input_map.size() % 2))
        {
            cLOG(Level::ERROR, "You entered replace contents with invalid arguments.");
            return;
        }
        /* Create replacement jobs  */
        std::map<std::string, std::string> replacements;
        for(auto it = input_map.find(1); it != input_map.end(); ++it)
        {
            auto joba = std::make_shared<open62541::Job>(input, JOB_REPLACE);
            std::string dummy = it->second;
            if (++it == input_map.end()) 
            {
                break;
            }
            auto jobb = nodeID_type(joba, dummy, "5");
            jobb->add_data(DATA_WRITE,
                    std::make_shared<open62541::DString>(it->second + "\n"));

            jobs_.push_back(jobb);
        }
    }
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
        cLOG(Level::ERROR, "Syntax error.");
        return result;
    }

    std::stringstream ss(input);
    std::string item;
    int index = 0;
    while(std::getline(ss, item, ':'))
    {
        if(item.empty())
        {
            cLOG(Level::ERROR, "Single argument is empty.");
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

/* Eof */