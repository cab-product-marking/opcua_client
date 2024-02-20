#pragma once 

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <memory>
 
#include "common_defines.h"
#include "os_wrappers.h"

#ifndef NDEBUG
/* Classic log */
#define cLOG(level, message)        Logger::get_instance().log(level, message)   
#endif

/* Job log */
#define jLOG(level, message, ptr)   Logger::get_instance().log(level, message, ptr)
/* Data log */
// #define dLOG

enum class Level
{
    INFO, 
    WARNING,
    ERROR,
    JOB,
    DATA
};

/* Forward declaration */
class Job;

/**
 * Class Logger
 * ^^^^^^^^^^^^
*/
class Logger
{
public:
    Logger();

    virtual 
    ~Logger();

    Logger(const Logger&) = delete;

    Logger&
    operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;

    Logger&
    operator=(Logger&&) = delete;

    /* Return static logger instance */
    static Logger& 
    get_instance(void);

    /* Basic log level parser */
    void 
    log(Level level, const std::string& message, std::shared_ptr<Job> jsptr = nullptr);

    /* Time function */
    std::string 
    actual_time(void);

};

/* Eof */