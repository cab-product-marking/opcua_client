#include "logger.h"

/* Return static logger instance */
Logger& 
Logger::get_instance(void)
{
    static Logger instance;
    return instance;
}

/* Basic log level parser */
void 
Logger::log(Level level, const std::string& message, opcuac::jobsptr job) 
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);
    std::string prefix = actual_time();
    
    switch(level) 
    {
        case Level::INFO:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_GREEN 
            << std::setw(20) << std::left << " info/cab" << FONT_RESET 
            << message 
            << std::endl;
            break;
        case Level::WARNING:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_YELLOW 
            << std::setw(20) << std::left << " warn/cab" << FONT_RESET 
            << message 
            << std::endl;
            break;
        case Level::ERROR:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_RED 
            << std::setw(20) << std::left << " error/cab" << FONT_RESET 
            << message 
            << std::endl;
            break;
        case Level::JOB:
            if(local == nullptr)
            {
                std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                << std::setw(20) << std::left << " job/cab" << FONT_RESET
                << message 
                << " | " 
                << "job nullptr!"
                << std::endl;
            }
            else
            {
                std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                << std::setw(20) << std::left << " job/cab" << FONT_RESET
                << message 
                << " | " 
                << local->get_info(PRAEFIX_INIT) 
                << std::endl;
            }
            break;
        case Level::DATA:
            if(local == nullptr)
            {
                std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                << std::setw(20) << std::left << " job/cab" << FONT_RESET
                << message 
                << " | " 
                << "job nullptr!"
                << std::endl;
            }
            else
            {
                auto obj = local->get_data(DATA_READ);
                auto data = static_cast<open62541::Data*>(obj.get());

                if(obj == nullptr)
                {
                    std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                    << std::setw(20) << std::left << " job/cab" << FONT_RESET
                    << message 
                    << " | " 
                    << "data nullptr!"
                    << std::endl;
                }
                else
                {
                    std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                    << std::setw(20) << std::left << " job/cab" << FONT_RESET
                    << message 
                    << " | " 
                    << data->type() 
                    << " - "
                    << *data
                    << std::endl;
                }
            }
            break;
    }
}

/* Time function */
std::string 
Logger::actual_time(void) 
{
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm now_tm;

    oswrapper::localtime(&now_as_time_t, &now_tm);

    std::ostringstream oss;

    oss << std::put_time(&now_tm, "[%Y-%m-%d %H:%M:%S.");
    oss << std::setfill('0') << std::setw(3) << now_ms.count();
    /* Fixed timezone */
    oss << " (UTC+0100)]"; 
    return oss.str();
}

/* Eof */