#include "logger.h"

Logger::Logger()
{

}

Logger::~Logger()
{

}

/* Return static logger instance */
Logger& 
Logger::get_instance(void)
{
    static Logger instance;
    return instance;
}

/* Basic log level parser */
void 
Logger::log(Level level, const std::string& message, open62541::jsptr job) 
{
    std::string prefix = actual_time();
    
    switch(level) 
    {
        case Level::INFO:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_GREEN 
            << std::setw(20) << std::left << " info/cab" << FONT_RESET 
            << message << std::endl;
            break;
        case Level::WARNING:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_YELLOW 
            << std::setw(20) << std::left << " warn/cab" << FONT_RESET 
            << message << std::endl;
            break;
        case Level::ERROR:
            std::cout << FONT_LIGHT_BLUE << prefix << FONT_RED 
            << std::setw(20) << std::left << " error/cab" << FONT_RESET 
            << message << std::endl;
            break;
        case Level::JOB:
            if(job == nullptr)
            {
                std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                << std::setw(20) << std::left << " job/cab" << FONT_RESET
                << "nullptr!" << " - " << message << std::endl;
            }
            else 
            {
                // std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                // << std::setw(20) << std::left << " job/cab" << FONT_RESET
                // << jsptr->init_string << " - " << message << std::endl;
                std::cout << "to do ..." << std::endl;
            }
            break;
        case Level::DATA:
            if(job == nullptr)
            {
                std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                << std::setw(20) << std::left << " job/cab" << FONT_RESET
                << "nullptr!" << " - " << message << std::endl;
            }
            else 
            {
                // std::cout << FONT_LIGHT_BLUE << prefix << FONT_BLUE
                // << std::setw(20) << std::left << " job/cab" << FONT_RESET
                // << jsptr->init_string << " - " << message << std::endl;
                std::cout << "to do ..." << std::endl;
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

    osw::wrapper_localtime(&now_as_time_t, &now_tm);

    std::ostringstream oss;

    oss << std::put_time(&now_tm, "[%Y-%m-%d %H:%M:%S.");
    oss << std::setfill('0') << std::setw(3) << now_ms.count();
    /* Fixed timezone */
    oss << " (UTC+0100)]"; 
    return oss.str();
}

/* Eof */